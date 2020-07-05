#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "stats.h"
#include "priority.h"
#include "tools.h"

static int compare(const void *a, const void *b);
static void assign(void *a, void *b);
static void ensureInvolved(Graph *src, int point);

typedef struct {
    unsigned long long pos;
    unsigned long long dis;
} vertexInfo;

static int dfs(Graph* this, int start, int end, int *pth, char *vis); 

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

int *_graphBFS(struct graph *this, int start, int end) {
	unsigned long long int* vertexDis = NULL;//记录最短路径
    char* vis = NULL;      //访问情况

	vertexDis = (unsigned long long int*) malloc(sizeof(unsigned long long int) * this->_vertexMax);
	vis = (char *) malloc(sizeof(char) * this->_vertexMax);
    int *res = (int *) malloc(sizeof(int) * this->_vertexMax);
    int *pre = (int *) malloc(sizeof(int) * this->_vertexMax); 
    int *queue = (int *) malloc(sizeof(int) * this->_vertexMax);

    for (int i = 0; i < this->_vertexMax; ++i) {
		vis[i] = 0;
		vertexDis[i] = -1;
        pre[i] = -1;
	}

	vertexDis[start] = 0;
	queue[0] = start;
    int queueFront = -1;
    int queueRear = 0;
    int queueCapicity = this->_vertexMax;
	vis[start] = 1;

	while (queueFront % queueCapicity < queueRear % queueCapicity) {	
        int now = queue[(++queueFront) % queueCapicity];	
		vis[now] = 0;                          //出队不再访问
		for (int id = this->_vertexList[now]; id != -1; id = this->_edgeList[id].nextID) {
			int next = this->_edgeList[id].to;
			if (vertexDis[next] > vertexDis[now] + this->_edgeList[id].weight) {
				vertexDis[next] = vertexDis[now] + this->_edgeList[id].weight;
                pre[next] = now;
				if (vis[next]) {
                    continue;     
                }
				vis[next] = 1;
				queue[(++queueRear) % queueCapicity] = next;
			}
		}

	}
    
    /*记录路径*/
    int r = 0;
    for(int x = end; x != start && pre[x] != -1; x = pre[x]) {
         res[r++] = x;                           //放在后面是正着存的，输出就是正着输出的
    }
    res[r] = start;
    res[r + 1] = -1;
    res[r + 2] = vertexDis[end];
    res[r + 3] = vertexDis[end] >> 32;

    free(vertexDis);
    free(vis);
    free(pre);
    free(queue);
	return res;
}

 int* graphBFS(char *filename, int start, int end)
 {
     if (!mainGraph) {
        initGraph(filename);
    }
    ensureInvolved(mainGraph, start);
    ensureInvolved(mainGraph, end);
    
    return mainGraph->vtable.graphBFS(mainGraph, start, end);
 }


/*Recursive auxiliary function*/
static int dfs(Graph* this, int start, int end, int *path, char *visit)
{
    static int flag = 0;
    static int t = 1;

	if (start == end) {
        path[t] = -1;
		return 1;
	}
    
    visit[start] = 1;

	for (int ID = this->_vertexList[start]; ID != -1; ID = this->_edgeList[ID].nextID) {
	    int currVertex = this->_edgeList[ID].to;
	    if (visit[currVertex] == 0) {
            visit[currVertex] = 1;
            path[t++] = currVertex;
            if (dfs(this, currVertex, end, path, visit)) {                 //Avoid unnecessary loops
                return 1;
            }
            path[--t] = 0;  
	    }
	}
    return 0;
}

//这几个算法也是graph类里面的吗
int *_graphDFS(Graph *this, int start, int end)
{
    char *visit = (char *) calloc(sizeof(char), this->_vertexMax);
    int *path = (int *) calloc(this->_vertexNum, sizeof(int));

    int ID = this->_vertexList[start];
	if (ID == -1) { 
        return NULL;
    }
	path[0] = start;

    int dfsRet = 0;

    asm(".intel_syntax noprefix\n");
    register char *r8 __asm__("r8") = visit;
    void *r9 = malloc(sizeof(long long) * 8 * this->_vertexNum);
    register void *newStack __asm__("r9") = r9 + sizeof(long long) * 8 * (this->_vertexNum - 1);
    register long long rsp __asm__("rsp");
    *(long long *)newStack = rsp;
    
    __asm__ __volatile__ (
         "push rbp\n"
         "mov rbp, r9\n"
         "mov rsp, rbp\n"
         "call rax\n"
         "pop rsp\n"
         "sub rsp, 0x8\n"
         "pop rbp\n"
         : "=a"(dfsRet)
         : "a"(dfs),
           "D"(this),
           "S"(start),
           "d"(end),
           "c"(path),
           "r"(r8),
           "r"(newStack)
         : "memory"
    );

	if (dfsRet == 0) {
        path[1] = -1;
    }
    free(r9);
    free(visit);
	return path;
}


int *graphDFS(char *filename, int start, int end)
{
   if (!mainGraph) {
        initGraph(filename);
    }
    ensureInvolved(mainGraph, start);
    ensureInvolved(mainGraph, end);
    return mainGraph->vtable.graphDFS(mainGraph, start, end);
}

//不知道这个函数如何传入文件，所以就先用maingraph了
int* shortestPath(int u, int v, char algorithm[], char filename[])
{
    int *shortPath;
    if (strcmp(algorithm, "DFS") == 0) {
        shortPath = graphDFS(filename, u, v);
    } else if (strcmp(algorithm, "BFS" ) == 0) {
        shortPath = graphBFS(filename, u, v);
    } else if (strcmp(algorithm, "BFS" ) == 0) {
        shortPath = graphDijkstar(filename, u, v);
    }
    return shortPath;
}


int* graphDijkstar(char *filename, int start, int end) {
    if (!mainGraph) {
        initGraph(filename);
    }
    ensureInvolved(mainGraph, start);
    ensureInvolved(mainGraph, end);

    return mainGraph->vtable.graphDijkstra(mainGraph, start, end);
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
    path[top + 1] = vertexDis[end];
    path[top + 2] = vertexDis[end] >> 32;

    free(vertexDis); 
    free(pre);
    free(vis);
    return path;
}

static int compare(const void *a, const void *b) {
    return (*(vertexInfo *)a).dis - (*(vertexInfo *)b).dis;
}

static void assign(void *a, void *b) {
    *(__int128 *)a = *(__int128 *)b;
}