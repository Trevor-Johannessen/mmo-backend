#include "include/tile.h"

int tile_event_activate(int x, int y, void *args){

}

int tile_check_space(int x, int y){

}

int tile_get_segments(int width){
    return width>>3 + (width&7 > 0); 
}

long tile_spawn_player(Tile *tile, Player *player){
    // places player in random open space
    // returns x/y cords packed in long
}

TileRow *tile_row_create(int width){
    int mod, segments;
    TileRow *row;
    segments = tile_get_segments(width);
    row = malloc(sizeof(TileRow)+sizeof(char)*segments);
}

void tile_row_free(int height, TileRow **rows){
    int i;
    for(i=0;i<height;i++)
        free(rows[i]);
}

Tile *tile_create(int id, int width, int height){
    Tile *tile;
    int i;

    tile = malloc(sizeof(Tile));
    tile->id = id;
    tile->height = height;
    tile->width = width;
    tile->players = 0x0;
    tile->refs = 0;

    // create collision
    tile->collision = malloc(sizeof(TileRow *) * height);
    for(i=0;i<height;i++)
        tile->collision[i] = tile_row_create(width);

    // create decor
    tile->decor = malloc(sizeof(char) * height * width);
    memset(tile->decor, 0, sizeof(char) * height * width);

    return tile;
}

void tile_event_free(TileEvent *event){
    
}

void tile_free(Tile *tile){
    Link *link, *parent;

    if(tile->refs--) // stupid way to write this line but it makes me feel cool :sunglasses:
        return;

    // free decor
    free(tile->decor);

    // free rows
    tile_row_free(tile->height, tile->collision);

    // free players
    link = tile->players;
    while(link){
        parent = link;
        link = link=link_next(link);
        player_free(parent->payload);
        free(parent);
    }

    // free events
    link = tile->events;
    while(link){
        parent = link;
        link = link=link_next(link);
        tile_event_free(parent->payload);
        free(parent);
    }

    free(tile);
}

Tile *tile_load(int id){
    // check if tile is not already initalize (is null)
    if(!tiles[id])
        tiles[id] = tile_loaders[id]();
    tiles[id]->refs+=1;
    return tiles[id];
}

void tile_unload(int id){
    if(!tiles[id])
        return;
    if(!--tiles[id])
        tile_free(tiles[id]);
    return;
}