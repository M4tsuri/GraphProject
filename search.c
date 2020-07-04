#include "graph.h"
#include "search.h"
#include <stdlib.h>
#include "stats.h"
#include "priority.h"
#include <stdio.h>

static int compare(const void *a, const void *b);
static void assign(void *a, void *b);
typedef struct {
    unsigned long long pos;
    unsigned long long dis;
} vertexInfo;

int _graphDFS(struct graph *this, int start, int end, int *res) {
    return 0;
}

int _graphBFS(struct graph *this, int start, int end, int *res) {
    return 0;
}

int* graphDijkstar(char *filename, int start, int end) {
    if (!mainGraph) {
        initGraph(filename);
    }

    int *path = NULL;
    mainGraph->vtable.graphDijkstra(mainGraph, start, end, path);
    return path;
}

int _graphDijkstra(struct graph *this, int start, int end, int *path) {
    unsigned long long int *vertexDis = NULL;
    vertexDis = (unsigned long long int *) malloc(sizeof(unsigned long long int) * this->_vertexMax);
    for (int i = 0; i < this->_vertexMax; ++i) {
        vertexDis[i] = -1;
    }

    priorityQueue *q = initPriorityQueue(this->_vertexMax, sizeof(vertexInfo), compare, assign);

    vertexDis[start] = 0;

    vertexInfo tmp = {
        .pos = start,
        .dis = vertexDis[start]
    };

    vertexInfo *curr = NULL;

    q->vtable.push(q, &tmp);
    while(q->vtable.size(q)){
        curr = q->vtable.pop(q);
        int from = curr->pos;
        for(int ID = this->_vertexList[from]; ID != -1 ; ID = this->_edgeList[ID].nextID) {
            if(vertexDis[this->_edgeList[ID].to] > this->_edgeList[ID].weight + curr->dis) {
                vertexDis[this->_edgeList[ID].to] = this->_edgeList[ID].weight + curr->dis;
                tmp.pos = this->_edgeList[ID].to;
                tmp.dis = vertexDis[this->_edgeList[ID].to];
                q->vtable.push(q, &tmp);
            }
        }
    }
    q->vtable.fini(q);

    int cur = end;
    int top = 1;
    path = (int *) calloc(this->_vertexNum, sizeof(int));
    path[0] = end;

    path[top] = -1;
    free(vertexDis);
    return top - 1;
}

static int compare(const void *a, const void *b) {
    return (*(vertexInfo *)a).dis - (*(vertexInfo *)b).dis;
}

static void assign(void *a, void *b) {
    *(__int128 *)a = *(__int128 *)b;
}