#ifndef SEARCH_H
#define SEARCH_H
#include "graph.h"

int _graphDFS(struct graph *, int, int, int *);
int _graphBFS(struct graph *, int, int, int *);
int _graphDijkstra(struct graph *, int, int, int *);

int *graphDFS(char *, int, int);
int *graphBFS(char *, int, int);
int *graphDijkstra(char *, int, int);

#endif