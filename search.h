#ifndef SEARCH_H
#define SEARCH_H
#include "graph.h"

int *_graphDFS(struct graph *, int, int);
int _graphBFS(struct graph *, int, int, int *);
int *_graphDijkstra(struct graph *, int, int);

/* 
 * input:
 *      graph pointer, start point, end point
 * side effects:
 *      None
 * return value:
 *      pointer to start and end path
 */
int* graphDFS(char *filename, int start, int end);

/* 
 * input: 
 *      graph pointer, start point
 * side effects:
 *      None
 * return value:
 *      pointer to the shortest path from start to end
 */
int* graphBFS(char *filename, int start, int end);

/* 
 * input: 
 *      start, end, traversal method
 * side effects:
 *      None
 * return value:
 *      pointer to the path from start to end
 */
int* shortestPath(int u, int v, char algorithm[]);

/* 
 * input: 
 *      graph pointer, start point, end point
 * side effects:
 *      None
 * return value:
 *      pointer to the shortest path from start to end
 */
int* graphDijkstar(char *filename, int start, int end);

#endif
