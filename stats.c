#include "stats.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include "search.h"
#include "tools.h"

/* some helper functions */
static void addEdge(graphEdges *, int *, int start, int end, int ID, int weight);
static void *safeMalloc(Graph *, size_t);
static void safeDestroy(Graph *, char *);
static int maxDegree(Graph *this);
static void initArray(int *start, int length, int num);

/* destructor of the whole graph, to make sure the malloc memory being freed */
void __attribute__((destructor)) finalDestroy();

/* they will be registered as class Graph's methods */
int _destroyGraph(Graph *);
int _numberOfEdges(Graph *);
int _numberOfVertices(Graph *);
float _freemanNetworkCentrality(Graph *);
float _closenessCentrality(Graph *, int node);


/* a instance of Graph used by all functions */
Graph *mainGraph = NULL;

static graphVtable graph_vtable = {
    .closenessCentrality = _closenessCentrality,
    .destroyGraph = _destroyGraph,
    .freemanNetworkCentrality = _freemanNetworkCentrality,
    .numberOfEdges = _numberOfEdges,
    .numberOfVertices = _numberOfVertices,
    .graphBFS = _graphBFS,
    .graphDFS = _graphDFS,
    .graphDijkstra = _graphDijkstra
};


/* initialize the while graph with the data specified in file */
Graph *initGraph(char *filename) {
    /* request space */
    Graph *productGraph = NULL;
    productGraph = (Graph *) safeMalloc(productGraph, sizeof(Graph));

    /* initialize method pointers */
    productGraph->_edgeNum = 0;
    productGraph->vtable = graph_vtable;

    /* open source file */
    int fd = 0;
    fd = SYS_OPEN((long long)filename, 0, 0);
    if (fd == -1) {
        safeDestroy(productGraph, "Open file failed");
    }

    /* get file length */
    long long int fileSize = SYS_LSEEK(fd, 0, 2);

    /* map source file into memory */
    long long int mapSize = (fileSize & ~((long long)0xfff)) + 0x1000;
    char *fileContent = (char *)SYS_MMAP(0, mapSize, 0x2, 0x2, fd, 0);
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
    SYS_MUNMAP((long long)fileContent, mapSize);
    SYS_CLOSE(fd);

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


/* initialize an array with a specified value num */
inline static void initArray(int *start, int length, int num) {
    for (int i = 0; i < length; ++i) {
        start[i] = num;
    }
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
    return mainGraph->vtable.numberOfVertices(mainGraph);;
}

void finalDestroy() {
    if (mainGraph) {
        mainGraph->vtable.destroyGraph(mainGraph);
    }
}

float _freemanNetworkCentrality(Graph *this) {
    int *degreeArray = (int *) calloc(sizeof(int), this->_vertexMax);
    int bidirFlag = 0;

    for (int i = 0; i < this->_vertexMax; i++) {
        int ID = this->_vertexList[i];
        for ( ; ID != -1; ID = this->_edgeList[ID].nextID) {
            degreeArray[i]++;
            int toID = this->_vertexList[this->_edgeList[ID].to];
            for ( ; toID != -1; toID = this->_edgeList[toID].nextID) {
                if (this->_edgeList[toID].to == i) {
                    bidirFlag = 1;
                    break;
                }
            }
            if (bidirFlag) {
                bidirFlag = 0;
                continue;
            }
            degreeArray[this->_edgeList[ID].to]++;
        }
    }

    int maxDegree = degreeArray[0];  
    for (int i = 1; i < this->_vertexMax; i++) {
        if (degreeArray[i] > maxDegree) {
            maxDegree = degreeArray[i];
        } 
    }

    unsigned long long int sum = 0;

    for (int i = 0; i < this->_vertexMax; ++i) {
        if (degreeArray[i] == 0) {
            continue;
        }
        sum += maxDegree - degreeArray[i];
    }
    
    unsigned long long vertexNum = this->_vertexNum;
    float center = (double)sum / (((double)vertexNum - 1) * ((double)vertexNum - 2));
    free(degreeArray);
    return center;
}

float _closenessCentrality(Graph *this, int node) {
    int sum = 0;
    int *path;
    int n = this->_vertexMax;
    //this->graphDijkstar(this, node, path);
    for (int i = 0; i < this->_vertexMax; i++) {
        sum += path[i];
    }
    float close = (n - 1) / sum;
    return close;
}

float freemanNetworkCentrality(char name[])
{
    if (!mainGraph) {
        initGraph(name);
    }
 
    return mainGraph->vtable.freemanNetworkCentrality(mainGraph);
}

float closenessCentrality(char name[], int node)
{
    if (!mainGraph) {
        initGraph(name);
    }
 
    return mainGraph->vtable.closenessCentrality(mainGraph, node);
}
