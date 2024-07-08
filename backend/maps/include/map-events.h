#ifndef MAPEVENTS_H
#define MAPEVENTS_H


typedef struct {
    struct player *player; // player who triggered the event
    struct map *map;
    struct map_event *event;
} MapEventArgs;

void map_event_hello(MapEventArgs *args);
void map_event_goodbye(MapEventArgs *args);
void map_event_change_map(MapEventArgs *args);

#include "../../player/include/player.h"

#endif