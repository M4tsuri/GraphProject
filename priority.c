#include <stdlib.h>
#include "priority.h"
#include <stdio.h>

static int (*compare)(const void *, const void *);
static void (*assign)(void *, void *);


static void *_pop(priorityQueue *);
static void _push(priorityQueue *, void *);
static int _isEmpty(priorityQueue *);
static int _isFull(priorityQueue *);
static int _size(priorityQueue *);
static void _fini(priorityQueue *);

static priorityQueueVtable pq_vtable = {
    .isEmpty = _isEmpty,
    .isFull = _isFull,
    .size = _size,
    .pop = _pop,
    .push = _push,
    .fini = _fini
};

void *minElement;
void *lastElement;

static void _fini(priorityQueue *this) {
    free(minElement);
    free(lastElement);
    free(this->_items);
    free(this);
}

static int _isFull(priorityQueue *this) {
    if (this->_size < this->_capacity) {
        return 0;
    }
    return 1;
}

static int _size(priorityQueue *this) {
    return this->_size;
}

static void *_pop(priorityQueue *this) {
    if (_isEmpty(this)) {
        printf("Error: No element in queue.\n");
        return 0;
    }

    assign(minElement, this->_items);
    assign(lastElement, this->_items + this->_itemSize * this->_size);
    this->_size--;

    int i = 0;
    int child = 0;
    for ( ; i * 2 <= this->_size; i = child) {
        child = i * 2 + (int)(i == 0);
        if (child < this->_size && 
            compare(this->_items + this->_itemSize * child, 
                    this->_items + this->_itemSize * (child + 1)) > 0) {
            child++;
        }

        if (compare(lastElement, this->_items + this->_itemSize * child) < 0) {
            break;
        } else {
            assign(this->_items + this->_itemSize * i, 
                   this->_items + this->_itemSize * child);
        }
    }
    assign(this->_items + this->_itemSize * i, lastElement);

    return minElement;
}

static void _push(priorityQueue *this, void *item) {
    if (this->vtable.isFull(this)) {
        printf("Error: Queue is full.\n");
        return;
    }

    if (this->_size == 0) {
        assign(this->_items + this->_itemSize * this->_size, item);
        this->_size = 1;
        return;
    }

    this->_size++;
    int i = this->_size;
    for ( ; compare(item, this->_items + this->_itemSize * (i / 2)) < 0; i /= 2) {
        assign(this->_items + this->_itemSize * i,
               this->_items + this->_itemSize * (i / 2));
    }
    assign(this->_items + this->_itemSize * i, item);
}

static int _isEmpty(priorityQueue *this) {
    if (this->_size) {
        return 0;
    }
    return 1;
}


priorityQueue *initPriorityQueue(int capacity, size_t itemSize, int (*cmp)(const void *, const void *), void (*assi)(void *, void *)) {
    priorityQueue *productQueue = NULL;
    productQueue = (priorityQueue *) malloc(sizeof(priorityQueue));

    /* initialize class method */
    productQueue->vtable = pq_vtable;

    /* initialize compare function for priority */
    compare = cmp;
    assign = assi;

    productQueue->_itemSize = itemSize;
    productQueue->_size = 0;
    productQueue->_capacity = capacity;
    productQueue->_items = malloc(itemSize * (capacity + 1));
    minElement = malloc(itemSize);
    lastElement = malloc(itemSize);
    if (!productQueue->_items) {
        printf("Error: Memory allocation failed.\n");
        exit(-1);
    }

    return productQueue;
}