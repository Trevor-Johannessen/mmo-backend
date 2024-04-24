#include "include/tile-definitions.h"

Tile *tiles[];
Tile *(*tile_loaders[])() = {
    tile_def_test_tile
};

Tile *tile_def_test_tile(){
    int height, width, i, j;
    char *decor_string;
    unsigned char *collision_string;
    TileRow **collision;
    TileEvent *events;
    Tile *tile;

    // define constants
    width = 64;
    height = 16;
    decor_string = "";
    collision_string = "";

    // create template
    tile = tile_create(0, width, height);

    // populate collision
    for(i=0; i<height;i++)
        for(j=0;j<tile_get_segments(width);j++)
            tile->collision[i]->segments[j] = collision_string[width*i+j];
    
    // populate decor
    strncpy(tile->decor, decor_string, width*height);

    // create events
    
    // return finished tile
    return tile;
}