#include "include/heap.h"

Heap *heap_create(void *object, void *identifier){
    Heap *heap;
    HeapNode *node;
    int offset;

    // create heap
    heap = malloc(sizeof(Heap));
    memset(heap, 0, sizeof(Heap));
    offset = identifier - object;

    // create new node
    node = malloc(sizeof(HeapNode));
    memset(heap, 0, sizeof(HeapNode));
    node->payload = object;
    node->value = object+offset;

    // add new node to heap
    heap->head = node;

    // return heap
    return heap;
}

int heap_get_offset(Heap *heap, HeapNode *node){
    return *((int *)(((char *)(node->payload))+heap->offset));
}

HeapNode *heap_compare_min(Heap *heap, HeapNode *l, HeapNode *r){
    int l_offset, r_offset;
    l_offset = heap_get_offset(heap, l);
    r_offset = heap_get_offset(heap, r);
    return l_offset < r_offset ? l : r;
}

HeapNode *heap_compare_max(Heap *heap, HeapNode *l, HeapNode *r){
    int l_offset, r_offset;
    l_offset = heap_get_offset(heap, l);
    r_offset = heap_get_offset(heap, r);
    return l_offset > r_offset ? l : r;
}

void *heap_peek(Heap *heap){
    return (heap->head)->payload;
}

void *heap_pop(Heap *heap, HeapNode *(*compare)(Heap *, HeapNode *, HeapNode *)){
    HeapNode *head;

    head = heap->head;
    heap->head = compare(heap, head->lchild, head->rchild);
    heap_delete(heap, head, compare);

    return head;
}

void heap_swap(HeapNode *parent, HeapNode *child){
    HeapNode *lchild, *rchild;

    lchild = parent->lchild;
    rchild = parent->rchild;
    parent->lchild = child->lchild;
    parent->rchild = child->rchild;
    if(lchild == child){
        child->lchild = parent;
        child->rchild = rchild;
    }else{
        child->lchild = lchild;
        child->rchild = parent;
    }
}

void heap_delete(Heap *heap, HeapNode *head, HeapNode *(*compare)(Heap *, HeapNode *, HeapNode *)){
    HeapNode *head_parent;
    while(head->lchild && head->rchild){
        head_parent = compare(heap, head->lchild, head->rchild);
        heap_swap(head, head_parent);
    }
    if(head->lchild){
        head_parent = head->lchild;
        heap_swap(head, head->lchild);
    }else if(head->rchild){
        head_parent = head->rchild;
        heap_swap(head, head->rchild);
    }
    if(head_parent->rchild && head_parent->rchild == head)
        head_parent->rchild = 0x0;
    else if(head_parent->lchild && head_parent->lchild == head)
        head_parent->lchild = 0x0;
}

void min_heap_insert(Heap *heap, void *object){
    
}

void *min_heap_pop(Heap *heap){
    return heap_pop(heap, heap_compare_min);
}

void max_heap_insert(Heap *heap, void *object){
    
}

void *max_heap_pop(Heap *heap){
    return heap_pop(heap, heap_compare_max);
}

void heap_destroy(Heap *heap){

}