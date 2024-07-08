
#ifndef MAPDEFS_H
#define MAPDEFS_H

#include "map.h"

#define MAP_COUNT 2
#define MAP_TEST 0
#define MAP_TEST_2 1


extern Map *tiles[MAP_COUNT];
extern Map *(*map_loaders[MAP_COUNT])();
extern pthread_mutex_t map_locks[MAP_COUNT];

// MAP INITALIZATION FUNCTIONS
Map *map_def_test_tile();
Map *map_def_test_tile_2();



#endif