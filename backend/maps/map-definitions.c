#include "include/map-definitions.h"

Map *tiles[];
Map *(*map_loaders[])() = {
    map_def_test_tile,
    map_def_test_tile_2
};
pthread_mutex_t map_locks[MAP_COUNT];

Map *map_def_test_tile(){
    int height, width, i, j, *change_map_args;
    char *decor_string;
    unsigned char *collision_string;
    TileRow **collision;
    Map *map;

    // define constants
    width = 32;
    height = 8;

    // create template
    map = map_create(MAP_TEST, width, height);

    // populate collision
    for(i=0; i<height;i++)
        for(j=0;j<map_get_segments(width);j++)
            map->collision[i]->segments[j] = 0;
    
    // populate decor
    memset(map->decor, '-', width*height);
    map->decor[width*6-1] = 'x';
    
    // create events
    map_add_event(map, map_event_create(5, 5, 0x0, map_event_hello));
    map_add_event(map, map_event_create(5, 5, 0x0, map_event_goodbye));
    change_map_args = malloc(sizeof(int)*4);
    change_map_args[0] = MAP_TEST_2;
    change_map_args[1] = 0;
    change_map_args[2] = 5;
    change_map_args[3] = 1;
    map_add_event(map, map_event_create(31, 5, change_map_args, map_event_change_map));

    // return finished map
    return map;
}

Map *map_def_test_tile_2(){
    int height, width, i, j, *change_map_args;
    char *decor_string;
    unsigned char *collision_string;
    TileRow **collision;
    Map *map;

    // define constants
    width = 24;
    height = 6;

    // create template
    map = map_create(MAP_TEST_2, width, height);

    // populate collision
    for(i=0; i<height;i++)
        for(j=0;j<map_get_segments(width);j++)
            map->collision[i]->segments[j] = 0;
    
    // populate decor
    memset(map->decor, '-', width*height);
    for(i=1;i<=height;i++)
        map->decor[width*i-1] = 'x';
    map->decor[width*5] = 'x';
    
    // create events
    map_add_event(map, map_event_create(5, 5, 0x0, map_event_hello));
    map_add_event(map, map_event_create(5, 5, 0x0, map_event_goodbye));
    change_map_args = malloc(sizeof(int)*4);
    change_map_args[0] = MAP_TEST;
    change_map_args[1] = 31;
    change_map_args[2] = 5;
    change_map_args[3] = 1;
    map_add_event(map, map_event_create(0, 5, change_map_args, map_event_change_map));

    // return finished map
    return map;
}