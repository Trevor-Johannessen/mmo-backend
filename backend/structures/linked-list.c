#include "include/linked-list.h"

Link *link_create(){
    Link *link;

    link = malloc(sizeof(Link));
    memset(link, 0, sizeof(Link));
    return link;
}

void link_free(Link *link){
    free(link);
}

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

// appends a new node to the front of the list and returns the new head
Link *link_add_first(Link *head, void *payload){
    Link *new_node;

    new_node = link_create();
    new_node->payload = payload;
    new_node->next = head;
    return new_node;
}

// appends a new node to the supplied node, returns the newly created node (tail)
Link *link_add_next(Link *head, void *payload){
    Link *new_node;

    new_node = link_create();
    new_node->payload = payload;
    if(head)
        head->next = new_node;
    return new_node;
}