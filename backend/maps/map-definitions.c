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
    TileEvent *events;
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
    memset(map->decor, 0, width*height);

    // create events
    
    // return finished map
    return map;
}