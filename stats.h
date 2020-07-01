#ifndef STATS_H
#define STATS_H

/* 
 * input: 
 *      filename
 * side effects:
 *      None
 * return value:
 *      number of edges in graph
 */
int numberOfEdges(char name[]);

/* 
 * input: 
 *      filename
 * side effects:
 *      None
 * return value:
 *      number of vertex in graph
 */
int numberOfVertices(char name[]);

/* 
 * input: 
 *      filename
 * side effects:
 *      None
 * return value:
 *      value of Freeman’s Network Centrality of graph
 */
int freemanNetworkCentrality(char name[]);

/* 
 * input: 
 *      filename
 * side effects:
 *      None
 * return value:
 *      value of Closeness Centrality of graph
 */
int closenessCentrality(char name[]);

#endif
