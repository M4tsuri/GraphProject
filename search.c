#include"search.h"
#include<stdio.h>
#include<stdlib.h>
#include "graph.h"
#include "search.h"
#include <stdlib.h>
#include "stats.h"
#include "priority.h"
#include <stdio.h>

static int compare(const void *a, const void *b);
static void assign(void *a, void *b);
static void ensureInvolved(Graph *src, int point);

typedef struct {
    unsigned long long pos;
    unsigned long long dis;
} vertexInfo;

static void dfs(Graph* this, int start, int end, int *pth, int *vis); 
void _graphBFS(struct graph *, int, int, int  *);
int *_graphDijkstra(struct graph *, int, int);

static void ensureInvolved(Graph *src, int point) {
    if (point >= src->_vertexMax) {
        printf("Error: vertex %d not in graph.\n", point);
        exit(-1);
    }
    if (src->_involvedVertices[point] == -1) {
        printf("Error: vertex %d not in graph.\n", point);
        exit(-1);
    }
}

/*Main function*/
int* graphDFS(char *filename, int start, int end);
int* graphBFS(char *filename, int start, int end);
int* graphDijkstar(char *filename, int start, int end);
int* shortestPath(int u, int v, char algorithm[]);

/*Recursive auxiliary function*/
static void dfs(Graph* this, int start, int end, int *path, int *visit)
{
    static int flag = 0;
    static int t = 0;
    static int ID;

	if (start == end) {
		for (int i = 0; i < t; i++) {
			printf("%d ", path[i]);           
		}
		flag = 1;                             //mark end
		return;
	}

	ID = this->_vertexList[start];
	if (ID == -1)return;                      //the start point is sink
	for (; this->_edgeList[ID].to != -1; ID = this->_edgeList[ID].nextID) {
	    int num = this->_edgeList[ID].to;
	    if (visit[num] != 1) {
            visit[num] = 1;
            path[t++] = num;
            dfs(this, num, end, path, visit);
            if (flag == 0) {                 //Avoid unnecessary loops
                visit[num] = 0;
                path[--t] = 0;
            } else return;
	    }
	}
}
int* _graphDFS(Graph *this, int start, int end)
{
    mainGraph = this;
    int ID = this->_vertexList[start];
	if (ID == -1)return NULL;
	int visit[this->_vertexMax];
    int path[this->_vertexMax];
    visit[start] = 1;
	path[0] = start;
	dfs(this, start, end, visit, path);
	return path;
}
    
 int* graphBFS(char *filename, int start, int end)
 {
     if (!mainGraph) {
        initGraph(filename);
    }
    ensureInvolved(mainGraph, start);
    ensureInvolved(mainGraph, end);
    int *path = NULL;
    
    return mainGraph->vtable.graphBFS(mainGraph, start, end, path);
 }

void _graphBFS(struct graph *this, int start, int end, int *res) {
{
	unsigned long long int* vertexDis = NULL;//记录最短路径
    unsigned long long int* vis = NULL;      //访问情况
	vertexDis = (unsigned long long int*) malloc(sizeof(unsigned long long int) * this->_vertexMax);
	vis = (unsigned long long int*) malloc(sizeof(unsigned long long int) * this->_vertexMax);
    int pre[this->_vertexMax];               //记录前驱路径

	vertexInfo tmp = {
		.pos = start,
		.dis = vertexDis[start]
	};

    for (int i = 0; i < this->_vertexMax; ++i) {
		vis[i] = 0;
		vertexDis[i] = 1e9;
        pre[i] = -1;
	}

	priorityQueue* q = initPriorityQueue(this->_vertexMax, sizeof(vertexInfo), compare, assign);
	vertexDis[start] = 0;
	vertexInfo* curr = NULL;
    res[this->_vertexMax-1] = start;
    int r = 0;                                 //记录res下标
	q->vtable.push(q, &tmp);
	vis[tmp.pos] = 1;

	while (!q->vtable.isEmpty(q)) {
		curr = q->vtable.pop(q);	
        int now = curr->pos;	
		vis[now] = 0;                          //出队不再访问
		for (int id= this->_vertexList[now]; id != -1; id = this->_edgeList[id].nextID) {
			int next = this->_edgeList[id].to;
			if (vertexDis[next] > vertexDis[now] + this->_edgeList[id].weight) {
				vertexDis[next] = vertexDis[now] + this->_edgeList[id].weight;
                pre[next] = now;
				if (vis[next])continue;     
				vis[next] = 1;
                tmp.pos = next;
                tmp.dis = vertexDis[this->_edgeList[id].to];
				q->vtable.push(q, &tmp);
			}
		}

	}
    
    /*记录路径*/
    for(int x = end; pre[x] != -1; x = pre[x]) {
         res[r++]=x;                           //放在后面是正着存的，输出就是正着输出的
    }

	q->vtable.fini(q);
    free(vertexDis);
    free(vis);
	return;
}
 


 int* graphDFS(char *filename, int start, int end)
{
   if (!mainGraph) {
        initGraph(filename);
    }
    ensureInvolved(mainGraph, start);
    ensureInvolved(mainGraph, end);
    int *path = NULL;
    return mainGraph->vtable.graphDFS(mainGraph, start, end, path);
}

int* shortestPath(int u, int v, char algorithm[], char filename)
{
    if (!mainGraph) {
        initGraph(filename);
    }
    int *shortPath;
    if (strcmp(algorithm, "DFS") == 0) {
        shortPath = DFS(mainGraph, u, v);
    } else if (strcmp(algorithm, "BFS" ) == 0) {
        shortPath = BFS(mainGraph, u, v);
    } else if (strcmp(algorithm, "BFS" ) == 0) {
        shortPath = Dijkstar(mainGraph, u, v);
    } 
    return shortPath;
}   

int *_graphDijkstra(struct graph *this, int start, int end) {
    unsigned long long int *vertexDis = NULL;
    vertexDis = (unsigned long long int *) malloc(sizeof(unsigned long long int) * this->_vertexMax);
    for (int i = 0; i < this->_vertexMax; ++i) {
        vertexDis[i] = -1;
    }
    int *pre = (int *) malloc(sizeof(int) * this->_vertexMax);
    char *vis = (char *) calloc(sizeof(char), this->_vertexMax);
    
    for (int i = 0; i < this->_vertexMax; ++i) {
        pre[i] = -1;
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
        do {
            curr = q->vtable.pop(q);
        } while (q->vtable.size(q) > 0 && vis[curr->pos] == 1);
        
        if (vis[curr->pos] == 1) {
            break;
        }
        
        vis[curr->pos] = 1;

        int from = curr->pos;
        for(int ID = this->_vertexList[from]; ID != -1 ; ID = this->_edgeList[ID].nextID) {
            if(vertexDis[this->_edgeList[ID].to] > this->_edgeList[ID].weight + curr->dis) {
                vertexDis[this->_edgeList[ID].to] = this->_edgeList[ID].weight + curr->dis;
                tmp.pos = this->_edgeList[ID].to;
                tmp.dis = vertexDis[this->_edgeList[ID].to];
                q->vtable.push(q, &tmp);
                pre[this->_edgeList[ID].to] = from;
            }
        }
    }
    q->vtable.fini(q);

    int cur = end;
    int top = 1;
    int *path = (int *) calloc(this->_vertexNum, sizeof(int));
    path[0] = end;

    for ( ; cur != start && pre[cur] != -1; ++top) {
        cur = pre[cur];
        path[top] = cur;
    }
    path[top] = -1;

    free(vertexDis); 
    return path;
}

static int compare(const void *a, const void *b) {
    return (*(vertexInfo *)a).dis - (*(vertexInfo *)b).dis;
}

static void assign(void *a, void *b) {
    *(__int128 *)a = *(__int128 *)b;
}
   