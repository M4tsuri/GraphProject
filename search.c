#include"search.h"
#include<stdio.h>
#include<stdlib.h>
#include "graph.h"

/* Variables needed for recursion */
int t = 0;
int ID;
int flag = 0;
int visit[mianGraph->_vertexMax];          
int path[mainGraph->_edgeNum];
void dfs(Graph* this, int start, int end); 

/*Main function*/
int* graphDFS(char *filename, int start, int end);
int* graphBFS(Graph *this, int start, int end);
int* graphDijkstar(Graph *this, int start, int end);
int* shortestPath(int u, int v, char algorithm[]);

/*Recursive auxiliary function*/
static void dfs(Graph* this, int start, int end)
{
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
            dfs(this, num, end);
            if (flag == 0) {                 //Avoid unnecessary loops
                visit[num] = 0;
                path[--t] = 0;
            } else return;
	    }
	}
}

//这几个算法也是graph类里面的吗
int* _graphDFS(Graph *this, int start, int end)
{
    mainGraph = this;
    ID = this->_vertexList[start];
	if (ID == -1)return;
	visit[start] = 1;
	path[t++] = start;
	dfs(this, start, end);
	return path;
}
int _graphBFS(struct graph *, int, int, int  *);
int _graphDijkstra(struct graph *, int, int, int *);

 int* graphDFS(char *filename, int start, int end)
{
   if (!mainGraph) {
        initGraph(filename);
    }
    return mainGraph->_graphDFS(mainGraph, start, end);
}

//不知道这个函数如何传入文件，所以就先用maingraph了
int* shortestPath(int u, int v, char algorithm[])
{
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

int _graphBFS(struct graph *this, int start, int end, int *res) {
    return 0;
}

int _graphDijkstra(struct graph *this, int start, int end, int *res) {
    return 0;
}