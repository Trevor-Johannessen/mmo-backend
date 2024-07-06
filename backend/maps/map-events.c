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