#include "include/tile-definitions.h"

Tile *tiles[];
Tile *(*tile_loaders[])() = {
    tile_def_test_tile
};
pthread_mutex_t tile_locks[TILE_COUNT];

Tile *tile_def_test_tile(){
    int height, width, i, j;
    char *decor_string;
    unsigned char *collision_string;
    TileRow **collision;
    TileEvent *events;
    Tile *tile;

    // define constants
    width = 32;
    height = 8;

    // create template
    tile = tile_create(0, width, height);

    // populate collision
    for(i=0; i<height;i++)
        for(j=0;j<tile_get_segments(width);j++)
            tile->collision[i]->segments[j] = 0;
    
    // populate decor
    memset(tile->decor, 0, width*height);

    // create events
    
    // return finished tile
    return tile;
}