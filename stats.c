#include "stats.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include "search.h"
#include "tools.h"


typedef struct {
    int start;
    int end;
} simpEdge;

/* some helper functions */
static void addEdge(graphEdges *, int *, int start, int end, int ID, int weight);
static void *safeMalloc(Graph *, size_t);
static void safeDestroy(Graph *, char *);
static int maxDegree(Graph *this);
static int edgeCmp(const void *a, const void *b);

/* destructor of the whole graph, to make sure the malloc memory being freed */
void __attribute__((destructor)) finalDestroy();

/* they will be registered as class Graph's methods */
int _destroyGraph(Graph *);
int _numberOfEdges(Graph *);
int _numberOfVertices(Graph *);
float _freemanNetworkCentrality(simpEdge *, int , int, int);
float _closenessCentrality(Graph *, int node);


/* a instance of Graph used by all functions */
Graph *mainGraph = NULL;

static graphVtable graph_vtable = {
    .destroyGraph = _destroyGraph,
    .numberOfEdges = _numberOfEdges,
    .numberOfVertices = _numberOfVertices,
    .graphBFS = _graphBFS,
    .graphDFS = _graphDFS,
    .graphDijkstra = _graphDijkstra,
    .closenessCentrality = _closenessCentrality
};

/* initialize the while graph with the data specified in file */
Graph *initGraph(char *filename) {
    /* request space */
    Graph *productGraph = NULL;
    productGraph = (Graph *) safeMalloc(productGraph, sizeof(Graph));

    /* initialize method pointers */
    productGraph->_edgeNum = 0;
    productGraph->vtable = graph_vtable;

    size_t fileSize = 0;
    int fd = 0;
    char *fileContent = mmapFile(filename, &fd, &fileSize);
    if (!fileContent) {
        safeDestroy(productGraph, "Open file failed");
    }
    char *fileEnd = fileContent + fileSize;
    /* points at file tail */

    /* get vertex max id and edge number in graph */
    int vertexMax = 0;
    int startNode = 0;
    int endNode = 0;
    int edgeWeight = 0;
    int edgeNum = 0;
    for (char *curPtr = fileContent; curPtr < fileEnd; ) {
        readTri(&curPtr, &startNode, &endNode, &edgeWeight);
        vertexMax = vertexMax > startNode ? vertexMax : startNode;
        vertexMax = vertexMax > endNode ? vertexMax : endNode;
        edgeNum++;
    }
    vertexMax++;
    productGraph->_vertexMax = vertexMax;
    productGraph->_edgeNum = edgeNum;

    /* allocate memory for edges and vertexes */
    productGraph->_edgeList = (graphEdges *) safeMalloc(productGraph, edgeNum * sizeof(graphEdges));
    productGraph->_vertexList = (int *) safeMalloc(productGraph, vertexMax * sizeof(int));
    productGraph->_involvedVertices = (int *) safeMalloc(productGraph, vertexMax * sizeof(int));

    /* initialize vertexList with -1 */
    initArray(productGraph->_vertexList, vertexMax, -1);
    initArray(productGraph->_involvedVertices, vertexMax, -1);
    

    /* read data from file and construct graph */
    int i = 0;
    for (char *curPtr = fileContent; curPtr < fileEnd; ++i) {
        readTri(&curPtr, &startNode, &endNode, &edgeWeight);
        addEdge(productGraph->_edgeList, productGraph->_vertexList, startNode, endNode, i, edgeWeight);
        productGraph->_involvedVertices[startNode] = 0;
        productGraph->_involvedVertices[endNode] = 0;
    }

    /* close file and munmap */
    munmapFile(fileContent, fd, fileSize);
    

    /* initialize main graph */
    mainGraph = productGraph;

    /* get totaly used vertex number */
    productGraph->_vertexNum = vertexMax;
    for (int i = 0; i < vertexMax; ++i) {
        if (productGraph->_involvedVertices[i] == -1) {
            productGraph->_vertexNum--;
        }
    }
    
    printf("Graph initialized.\n");
    return productGraph;
}

/* destroy the graph in a safe way */
int _destroyGraph(Graph *this) {
    if (this->_edgeList) {
        free(this->_edgeList);
    }
    if (this->_vertexList) {
        free(this->_vertexList);
    }
    if (this->_involvedVertices) {
        free(this->_involvedVertices);
    }
    if (this) {
        free(this);
    }
    mainGraph = NULL;
    printf("Graph destroyed.\n");
    return 0;
}


/* display a error string and exit */
static void safeDestroy(Graph *this, char *errstr) {
    perror(errstr);
    if (this) {
        this->vtable.destroyGraph(this);
    }
    exit(-1);
}

/* malloc in a safer way */
static void *safeMalloc(Graph *this, size_t count) {
    void *res = malloc(count);
    if (res == NULL) {
        safeDestroy(this, "Memory allocation failed");
    }
    return res;
}

/* add an edge in graph */
inline static void addEdge(graphEdges *edgeList, int *vertexList, int start, int end, int ID, int weight) {
    edgeList[ID].to = end;
    edgeList[ID].weight = weight;
    edgeList[ID].nextID = vertexList[start];
    vertexList[start] = ID;
}


int _numberOfEdges(Graph *this) {
    return this->_edgeNum;
}

int numberOfEdges(char name[]) {
    if (!mainGraph) {
        initGraph(name);
    }
    return mainGraph->vtable.numberOfEdges(mainGraph);
}

int _numberOfVertices(Graph *this) {
    return this->_vertexNum;
}

int numberOfVertices(char name[]) {
    if (!mainGraph) {
        initGraph(name);
    }
    return mainGraph->vtable.numberOfVertices(mainGraph);
}

void finalDestroy() {
    if (mainGraph) {
        mainGraph->vtable.destroyGraph(mainGraph);
    }
}

static int edgeCmp(const void *a, const void *b) {
    simpEdge *m = (simpEdge *)a;
    simpEdge *n = (simpEdge *)b;
    long long int tmp = 0;
    
    long long p = m->start;
    long long q = n->start;
    p <<= 32;
    q <<= 32;
    p += m->end;
    q += n->end;

    long long res = p - q;

    if (res > 0) {
        return 1;
    }
    if (res < 0) {
        return -1;
    }
    return res;
}

float _freemanNetworkCentrality(simpEdge *src, int edgeNum, int vertexMax, int vertexNum) {
    qsort(src, edgeNum, sizeof(simpEdge), edgeCmp);
    int *degreeArray = (int *) malloc(vertexMax * sizeof(int));
    initArray(degreeArray, vertexMax, 0);

    for (int i = 0; i < edgeNum - 1; ++i) {
        if (src[i].end == src[i + 1].end && src[i].start == src[i + 1].start) {
            continue;
        }
        degreeArray[src[i].end]++;
        degreeArray[src[i].start]++;
    }
    degreeArray[src[edgeNum - 1].end]++;
    degreeArray[src[edgeNum - 1].start]++;

    int maxDegree = degreeArray[0];  
    for (int i = 1; i < vertexMax; i++) {
        if (degreeArray[i] > maxDegree) {
            maxDegree = degreeArray[i];
        } 
    }

    unsigned long long int sum = 0;

    for (int i = 0; i < vertexMax; ++i) {
        if (degreeArray[i] == 0) {
            continue;
        }
        sum += maxDegree - degreeArray[i];
    }
    
    unsigned long long N = vertexNum;

    float center = (double)sum / (((double)vertexNum - 1) * ((double)vertexNum - 2));
    free(degreeArray);
    return center;
}

float _closenessCentrality(Graph *this, int node) {
    unsigned long long *disArray = NULL;
    unsigned long long sum = 0;
    unsigned long long n = 0;
    this->vtable.graphDijkstra(this, node, -1, &disArray);
    for (int i = 0; i < this->_vertexMax; i++) {
        if ((long long)(disArray[i]) == -1) {
            continue;
        }
        sum += disArray[i];
        n++;
    }
    free(disArray);
    float close = ((double)n - 1) / (double)sum;
    return close;
}

float freemanNetworkCentrality(char name[])
{
    size_t fileSize = 0;
    int fd = 0;
    char *fileContent = mmapFile(name, &fd, &fileSize);
    if (!fileContent) {
        printf("Error: Open file failed.\n");
        exit(0);
    }
    char *fileEnd = fileContent + fileSize;

    int lineNum = 0;
    for (int i = 0; i < fileSize; ++i) {
        if (fileContent[i] == '\n') {
            lineNum++;
        }
    }

    simpEdge *allEdges = (simpEdge *) malloc(sizeof(simpEdge) * lineNum);

    int edgeNum = 0;
    int vertexMax = 0;
    int tmp = 0;
    int u = 0;
    int v = 0;

    for (char *curPtr = fileContent; curPtr < fileEnd; ) {
        readTri(&curPtr, &u, &v, &tmp);
        if (u > v) {
            allEdges[edgeNum].start = v;
            allEdges[edgeNum].end = u;
            vertexMax = vertexMax > u ? vertexMax : u;
        } else {
            allEdges[edgeNum].start = u;
            allEdges[edgeNum].end = v;
            vertexMax = vertexMax > v ? vertexMax : v;
        }
        edgeNum++;
    }
    vertexMax++;

    int *involvedVertices = (int *) malloc(vertexMax * sizeof(int));
    initArray(involvedVertices, vertexMax, -1);

    for (char *curPtr = fileContent; curPtr < fileEnd; ) {
        readTri(&curPtr, &u, &v, &tmp);
        involvedVertices[u] = 0;
        involvedVertices[v] = 0;
    }

    int vertexNum = vertexMax;
    for (int i = 0; i < vertexMax; ++i) {
        if (involvedVertices[i] == -1) {
            vertexNum--;
        }
    }

    free(involvedVertices);
    munmapFile(fileContent, fd, fileSize);
 
    float res = _freemanNetworkCentrality(allEdges, edgeNum, vertexMax, vertexNum);

    free(allEdges);
    return res;
}

float closenessCentrality(char name[], int node) {
    if (!mainGraph) {
        initGraph(name);
    }
    ensureInvolved(mainGraph, node);
    return mainGraph->vtable.closenessCentrality(mainGraph, node);
}