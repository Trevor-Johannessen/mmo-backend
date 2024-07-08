#include "include/map-definitions.h"

Map *tiles[];
Map *(*map_loaders[])() = {
    map_def_test_tile
};
pthread_mutex_t map_locks[MAP_COUNT];

Map *map_def_test_tile(){
    int height, width, i, j;
    char *decor_string;
    unsigned char *collision_string;
    TileRow **collision;
    Map *map;

    // define constants
    width = 32;
    height = 8;

    // create template
    map = map_create(0, width, height);

    // populate collision
    for(i=0; i<height;i++)
        for(j=0;j<map_get_segments(width);j++)
            map->collision[i]->segments[j] = 0;
    
    // populate decor
    memset(map->decor, '-', width*height);
    map->decor[5+width*5] = 'x';
    
    // create events
    map_add_event(map, map_event_create(5, 5, map_event_hello));
    map_add_event(map, map_event_create(5, 5, map_event_goodbye));
    map_add_event(map, map_event_create(10, 5, map_event_hello));
    

    // return finished map
    return map;
}