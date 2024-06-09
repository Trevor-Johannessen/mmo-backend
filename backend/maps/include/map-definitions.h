
#ifndef MAPDEFS_H
#define MAPDEFS_H

#include "map.h"

#define MAP_COUNT 1
#define MAP_TEST 0

extern Map *tiles[MAP_COUNT];
extern Map *(*map_loaders[MAP_COUNT])();
extern pthread_mutex_t map_locks[MAP_COUNT];

// MAP INITALIZATION FUNCTIONS
Map *map_def_test_tile();



#endif