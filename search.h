#ifndef STATS_H
#define STATS_H

/* 
 * input:
 *      graph pointer, start point, end point
 * side effects:
 *      None
 * return value:
 *      pointer to start and end path
 */
int* DFS(Graph *this, int start, int end);

/* 
 * input: 
 *      graph pointer, start point
 * side effects:
 *      None
 * return value:
 *      pointer to the shortest path from start to end
 */
int* BFS(Graph *this, int start, int end);

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
int* Dijkstar(Graph *this, int start, int end);

#endif
