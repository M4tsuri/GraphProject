#ifndef PRIORITY_H
#define PRIORITY_H
#include <stdlib.h>

struct heapq;


typedef struct {
    int (*isEmpty)(struct heapq *);
    void (*push)(struct heapq *, void *);
    void *(*pop)(struct heapq *);
    int (*size)(struct heapq *);
    int (*isFull)(struct heapq *);
    void (*fini)(struct heapq *);
} priorityQueueVtable;


typedef struct heapq {
    int _size;
    int _capacity;
    size_t _itemSize;
    void *_items;

    priorityQueueVtable vtable;
} priorityQueue;

priorityQueue *initPriorityQueue(int capacity, size_t itemSize, int (*)(const void *, const void *), void (*)(void *, void *));

#endif