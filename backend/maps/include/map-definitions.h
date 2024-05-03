
#ifndef MAPDEFS_H
#define MAPDEFS_H

#include "map.h"

#define TILE_COUNT 1

extern Map *tiles[TILE_COUNT];
extern Map *(*tile_loaders[TILE_COUNT])();
extern pthread_mutex_t tile_locks[TILE_COUNT];

// MAP INITALIZATION FUNCTIONS
Map *tile_def_test_tile();



#endif