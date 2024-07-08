#include "include/map-events.h"
#include <stdio.h>

void map_event_hello(MapEventArgs *args){
    if(args->player)
        fprintf(stdout, "Hello %s! :D\n", args->player->name);
    else
        fprintf(stdout, "Hello there! :D\n");
}

void map_event_goodbye(MapEventArgs *args){
    if(args->player)
        fprintf(stdout, "Goodbye %s!\n", args->player->name);
    else
        fprintf(stdout, "Goodbye!\n");
}

void map_event_change_map(MapEventArgs *args){
    int id, x, y, suppress;
    id = *((int *)args->event->static_args);
    x = *((int *)args->event->static_args+1);
    y = *((int *)args->event->static_args+2);
    suppress = *((int *)args->event->static_args+2);
    map_spawn_player(id, args->player, x, y, suppress);
}