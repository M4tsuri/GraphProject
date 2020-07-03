#include "stats.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include "search.h"

/* a instance of Graph used by all functions */
Graph *mainGraph = NULL;

/* some helper functions */
static void addEdge(Graph *src, int start, int end, int ID, int weight);
static void readTri(char **line, int *a, int *b, int *c);
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
static inline __attribute__((always_inline))
long long syscall(long long n, long long a1, 
                  long long a2, long long a3, 
                  long long a4, long long a5, 
                  long long a6);


#define SYS_OPEN(filename, flags, mode) \
    syscall(2, (filename), (flags), (mode), 0, 0, 0);

#define SYS_LSEEK(fd, offset, origin) \
    syscall(8, (fd), (offset), (origin), 0, 0 ,0);

#define SYS_MMAP(addr, len, prot, flags, fd, off) \
    syscall(9, (addr), (len), (prot), (flags), (fd), (off))

#define SYS_MUNMAP(addr, len) \
    syscall(11, addr, len, 0, 0, 0, 0)

#define SYS_CLOSE(fd) \
    syscall(3, fd, 0, 0, 0, 0, 0)


/* initialize the while graph with the data specified in file */
Graph *initGraph(char *filename) {
    /* request space */
    Graph *productGraph = NULL;
    productGraph = (Graph *) safeMalloc(productGraph, sizeof(Graph));

    /* initialize method pointers */
    productGraph->_edgeNum = 0;
    productGraph->closenessCentrality = _closenessCentrality;
    productGraph->destroyGraph = _destroyGraph;
    productGraph->freemanNetworkCentrality = _freemanNetworkCentrality;
    productGraph->numberOfEdges = _numberOfEdges;
    productGraph->numberOfVertices = _numberOfVertices;
    productGraph->graphBFS = _graphBFS;
    productGraph->graphDFS = _graphDFS;
    productGraph->graphDijkstra = _graphDijkstra;

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
        addEdge(productGraph, startNode, endNode, i, edgeWeight);
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
    return 0;
}

/* function for making a syscall link open and mmap */
static inline __attribute__((always_inline))
long long syscall(long long n, 
                  long long a1, long long a2, 
                  long long a3, long long a4, 
                  long long a5, long long a6) {
    long long ret;
    register long long r8 __asm__("r8") = a5;
    register long long r9 __asm__("r9") = a6;
    register long long r10 __asm__("r10") = a4;
    __asm__ __volatile__ (
        "syscall\n\t"
        : "=a"(ret)
        : "a"(n),
          "D"(a1),
          "S"(a2),
          "d"(a3),
          "r"(r10),
          "r"(r8),
          "r"(r9)
        : "memory",
          "rcx",
          "r11"
    );
    return ret;
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
inline static void readTri(char **line, int *a, int *b, int *c) {
    char *tmp = NULL;
    for (tmp = *line; *tmp != ' ' && *tmp != 0; ++tmp);
    *tmp = 0;
    *a = atoi(*line);
    *line = tmp + 1;
    for (tmp = *line; *tmp != ' ' && *tmp != 0; ++tmp);
    *tmp = 0;
    *b = atoi(*line);
    *line = tmp + 1;
    for (tmp = *line; *tmp != '\n' && *tmp != 0; ++tmp);
    *tmp = 0;
    *c = atoi(*line);
    *line = tmp + 1;
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
    float maxD = (float)maxDegree(this);
    float vertexNum = this->_vertexNum;
    float center = (vertexNum * maxD) / ((vertexNum - 1) * (vertexNum - 2));
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

/*Calculate the maximum degree of the graph*/
static int maxDegree(Graph *this)
{
    int temp[this->_vertexMax];
    initArray(temp, this->_vertexMax, 0);
	int D;

    for (int i = 0; i < this->_vertexMax; i++) {
        int ID = this->_vertexList[i];
        temp[i] += (int)(ID != -1);
        for ( ; ID != -1; ID = this->_edgeList[ID].nextID) {
            temp[this->_edgeList[ID].to]++;
        }
    }

    int maxDegree = temp[0];  
    for (int i = 1; i < this->_vertexNum; i++) {
        if (temp[i] > maxDegree) {
            maxDegree = temp[i];
        } 
    }
    
	return maxDegree;
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
 
    return mainGraph->closenessCentrality(mainGraph, node);
}
