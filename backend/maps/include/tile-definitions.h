#ifndef TILEDEFS_H
#define TILEDEFS_H

#include "tile.h"

#define TILE_COUNT 1

Tile *tiles[TILE_COUNT];
extern Tile *(*tile_loaders[TILE_COUNT])();

// TILE INITALIZATION FUNCTIONS
Tile *tile_def_test_tile();



#endif