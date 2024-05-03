#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>
#include <string.h>

#define min_heap_peek heap_peek
#define max_heap_peek heap_peek

typedef struct heap_node {
//    struct heap_node *parent;
    struct heap_node *lchild;
    struct heap_node *rchild;
    int *value;
    void *payload;
} HeapNode;

typedef struct heap {
    HeapNode *head;
    int offset;
} Heap;

Heap *heap_create(void *object, void *identifier);
void *heap_peek(Heap *heap);
void heap_delete(Heap *heap, HeapNode *head, HeapNode *(*compare)(Heap *, HeapNode *, HeapNode *));
void heap_destroy(Heap *heap);
void min_heap_insert(Heap *heap, void *object);
void *min_heap_pop(Heap *heap);
void max_heap_insert(Heap *heap, void *object);
void *max_heap_pop(Heap *heap);

#endif