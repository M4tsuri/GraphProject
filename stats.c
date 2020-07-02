#include "stats.h"
#include <stdio.h>
#include <stdlib.h>

/* point to an instance of Graph class */
Graph *mainGraph;

/* some helper functions */
static void addEdge(graphEdges *edgeList, int *edges, int start, int end, int ID, int weight);
static void readTri(char *line, int *a, int *b, int *c);
static void *safeMalloc(Graph *, size_t);
static void safeDestroy(Graph *, char *);

/* destructor of the whole graph, to make sure the malloc memory being freed */
void __attribute__((destructor)) finalDestroy();

/* they will be registered as class Graph's methods */
int _destroyGraph(Graph *);
int _numberOfEdges(Graph *);
int _numberOfVertices(Graph *);
float _freemanNetworkCentrality(Graph *);
float _closenessCentrality(Graph *, int node);

/* initialize the while graph with the data specified in file */
Graph *initGraph(char *filename) {
    /* request space */
    Graph *productGraph = NULL;
    productGraph = (Graph *) safeMalloc(productGraph, sizeof(Graph));

    /* initialize method pointers */
    productGraph->closenessCentrality = _closenessCentrality;
    productGraph->destroyGraph = _destroyGraph;
    productGraph->freemanNetworkCentrality = _freemanNetworkCentrality;
    productGraph->numberOfEdges = _numberOfEdges;
    productGraph->numberOfVertices = _numberOfVertices;

    /* open source file */
    FILE *srcFile = NULL;
    srcFile = fopen(filename, "r");
    if (srcFile == NULL){
        safeDestroy(productGraph, "Open file failed");
    }

    /* get vertex number and edge number in graph */
    char lineBuf[100] = {0};
    int edgeNum = 0;
    int vertexNum = 0;
    int startNode = 0;
    int endNode = 0;
    int edgeWeight = 0;
    for ( ; fgets(lineBuf, 100, srcFile); ++edgeNum) {
        readTri(lineBuf, &startNode, &endNode, &edgeWeight);
        vertexNum = vertexNum >= startNode ? vertexNum : startNode;
        vertexNum = vertexNum >= endNode ? vertexNum : endNode;
    }
    vertexNum++;
    productGraph->_vertexNum = vertexNum;
    productGraph->_edgeNum = edgeNum;

    /* allocate memory for edges and vertexes */
    productGraph->_edgeList = (graphEdges *) safeMalloc(productGraph, 
                                                        sizeof(graphEdges) * productGraph->_edgeNum);
    productGraph->_vertexList = (int *) safeMalloc(productGraph,
                                                   sizeof(int) * productGraph->_vertexNum);

    /* initialize vertexList with -1 */
    for (int i = 0; i < productGraph->_vertexNum; ++i) {
        productGraph->_vertexList[i] = -1;
    }

    /* read data from file and construct graph */
    rewind(srcFile);
    for (int i = 0; i < productGraph->_edgeNum; ++i) {
        fgets(lineBuf, 100, srcFile);
        readTri(lineBuf, &startNode, &endNode, &edgeWeight);
        addEdge(productGraph->_edgeList, productGraph->_vertexList,
                startNode, endNode, i, edgeWeight);
    }

    /* initialize main graph */
    mainGraph = productGraph;
    
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
    if (this) {
        free(this);
    }
    mainGraph = NULL;
    return 0;
}

/* display a error string and exit */
static void safeDestroy(Graph *this, char *errstr) {
    perror(errstr);
    if (this) {
        this->destroyGraph(this);
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
static void addEdge(graphEdges *edgeList, int *vertexList, int start, int end, int ID, int weight) {
    edgeList[ID].to = end;
    edgeList[ID].weight = weight;
    edgeList[ID].nextID = vertexList[start];
    vertexList[start] = ID;
}

/* read three numbers from string and store it in a, b, c */
static void readTri(char *line, int *a, int *b, int *c) {
    char *tmp = NULL;
    for (tmp = line; *tmp != ' '; ++tmp);
    *tmp = 0;
    *a = atoi(line);
    line = tmp + 1;
    for (tmp = line; *tmp != ' '; ++tmp);
    *tmp = 0;
    *b = atoi(line);
    line = tmp + 1;
    for (tmp = line; *tmp != '\n'; ++tmp);
    *tmp = 0;
    *c = atoi(line);
}

int _numberOfEdges(Graph *this) {
    return this->_edgeNum;
}

int numberOfEdges(char name[]) {
    if (!mainGraph) {
        initGraph(name);
    }
    int res = mainGraph->numberOfEdges(mainGraph);
    return res;
}

int _numberOfVertices(Graph *this) {
    return this->_vertexNum;
}

int numberOfVertices(char name[]) {
    if (!mainGraph) {
        initGraph(name);
    }
    int res = mainGraph->numberOfVertices(mainGraph);
    return res;
}

void finalDestroy() {
    if (mainGraph) {
        mainGraph->destroyGraph(mainGraph);
    }
}

float _freemanNetworkCentrality(Graph *this) {
    return 0.0;
}

float _closenessCentrality(Graph *this, int node) {
    return 0.0;
}

//freemanNetworkCentrality
int maxDegree(Graph *mainGraph)
{
    int *temp = (int)calloc(mainGraph ->_vertexNum, sizeof(int));
	int D;

    for (int i = 0; i < mainGraph->_edgeNum; i++) {
    int next = mainGraph->_edgeList[i].nextID;
    int now =  mainGraph->_edgeList[i].to;
    temp[now]++;
    temp[next]++;           
    } 

    int maxD = temp[0];  
    for (int i = 1; i < mainGraph->_vertexNum; i++) {
        if (D > maxD) {
            maxD = D;
        } 
    }
    
    free(temp);
	return maxD;
}

float freemanNetworkCentrality(char name[])
{
    if (!mainGraph) {
        initGraph(name);
    }
    int max = maxDegree(mainGraph);
    int n = mainGraph->_vertexNum;
    int m = mainGraph->_edgeNum;
    float center = (n * max) / ((n - 1)(n - 2));
    return center;
}