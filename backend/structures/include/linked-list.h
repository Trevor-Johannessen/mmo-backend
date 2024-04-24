#include <string.h>
#include <stdlib.h>

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct link {
    struct link *next;
    void *payload;
} Link;

//#DEFINE linked_list_iterate(x) for(x=0x0; link_next(x); x=link_next(x))

Link *link_next(Link *cur);
int link_has_next(Link *cur);
void link_map(Link *head, void (*func)(void *));
int link_contains(Link *head, void *item);
Link *link_add_next(Link *head, void *payload);
Link *link_add_first(Link *head, void *payload);

#endif