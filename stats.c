#include "stats.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include "search.h"

/* a instance of Graph used by all functions */
Graph *mainGraph = NULL;

/* some helper functions */
static void addEdge(Graph *src, int start, int end, int ID, int weight);
static void readTri(char *line, int *a, int *b, int *c);
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
    initArray(productGraph->_vertexList, productGraph->_vertexNum, -1);

    /* read data from file and construct graph */
    rewind(srcFile);
    for (int i = 0; i < productGraph->_edgeNum; ++i) {
        fgets(lineBuf, 100, srcFile);
        readTri(lineBuf, &startNode, &endNode, &edgeWeight);
        addEdge(productGraph, startNode, endNode, i, edgeWeight);
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
inline static void addEdge(Graph *src, int start, int end, int ID, int weight) {
    src->_edgeList[ID].to = end;
    src->_edgeList[ID].weight = weight;
    src->_edgeList[ID].nextID = src->_vertexList[start];
    src->_vertexList[start] = ID;
}

/* read three numbers from string and store it in a, b, c */
inline static void readTri(char *line, int *a, int *b, int *c) {
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
    return mainGraph->numberOfEdges(mainGraph);
}

int _numberOfVertices(Graph *this) {
    return this->_vertexNum;
}

int numberOfVertices(char name[]) {
    if (!mainGraph) {
        initGraph(name);
    }
    return mainGraph->numberOfVertices(mainGraph);;
}

void finalDestroy() {
    if (mainGraph) {
        mainGraph->destroyGraph(mainGraph);
    }
}

float _freemanNetworkCentrality(Graph *this) {
    int max = maxDegree(this);
    int n = this->_vertexNum;
    int m = this->_edgeNum;
    float center = (n * max) / ((n - 1) * (n - 2));
    return center;
}

float _closenessCentrality(Graph *this, int node) {
    int sum = 0;
    int *path;
    int n = this->_vertexNum;
    Dijkstar(this, node, path);
    for (int i = 0; i < this->_vertexNum; i++) {
        sum += path[i];
    }
    float close = (n - 1) / sum;
    return close;
}

/*Calculate the maximum degree of the graph*/
static int maxDegree(Graph *this)
{
    int temp[this->_vertexNum];
    initArray(temp, this->_vertexNum, 0);
	int D;

    for (int j = 0; j < this->_vertexNum; j++) {
        for (int i = 0; i < this->_edgeNum; i++) {
            int next = this->_edgeList[i].nextID;
            int now =  this->_edgeList[i].to;
            temp[now]++;
            temp[next]++;           
        }
    }

    int maxD = temp[0];  
    for (int i = 1; i < this->_vertexNum; i++) {
        if (temp[i] > maxD) {
            maxD = temp[i];
        } 
    }
    
	return maxD;
}

float freemanNetworkCentrality(char name[])
{
    if (!mainGraph) {
        initGraph(name);
    }
 
    return mainGraph->freemanNetworkCentrality(mainGraph);
}

float closenessCentrality(char name[], int node)
{
    if (!mainGraph) {
        initGraph(name);
    }
 
    return mainGraph->closenessCentrality(mainGraph);
}
