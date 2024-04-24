#include "include/linked-list.h"

Link *link_next(Link *cur){
    if(cur->next)
        return cur->next;
    return 0x0;
}

int link_has_next(Link *cur){
    if(!cur)
        return 0;
    return cur->next != 0x0;
}

void link_map(Link *head, void (*func)(void *)){
    if(!head)
        return;
    for(; link_has_next(head); head=link_next(head))
        (*func)(head->payload);
}

int link_contains(Link *head, void *item){
    if(!head)
        return 0;
    for(; link_has_next(head); head=link_next(head))
        if(head->payload == item)
            return 1;
    return 0;
}

Link *link_add_first(Link *head, void *payload){
    Link *new_node;

    new_node = malloc(sizeof(new_node));
    memset(new_node, 0, sizeof(new_node));
    new_node->payload = payload;
    new_node->next = head;
    return new_node;
}

Link *link_add_next(Link *head, void *payload){
    Link *new_node;

    new_node = malloc(sizeof(new_node));
    memset(new_node, 0, sizeof(new_node));
    new_node->payload = payload;
    if(head)
        head->next = new_node;
    return new_node;
}