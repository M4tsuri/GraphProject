#ifndef STATS_H
#define STATS_H

typedef struct edges {
    int to;
    int nextID;
    int weight;
} graphEdges;

#ifndef GRAPH
#define GRAPH

typedef struct graph {
    graphEdges *_edgeList;
    int *_vertexList;
    int _edgeNum;
    int _vertexNum;

    /* destructor of the class */
    int (*destroyGraph)(struct graph *);

    /* they are implementations of some basic operations on graph */
    int (*numberOfEdges)(struct graph *);
    int (*numberOfVertices)(struct graph *);
    float (*freemanNetworkCentrality)(struct graph *);
    float (*closenessCentrality)(struct graph *, int);
} Graph;
#endif

/* factory function to product graph class from file input */
Graph *initGraph(char *);

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
float freemanNetworkCentrality(char name[]);

/* 
 * input: 
 *      filename
 * side effects:
 *      None
 * return value:
 *      value of Closeness Centrality of graph
 */
float closenessCentrality(char name[], int node);

#endif
