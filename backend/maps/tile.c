#include "include/tile.h"
#include "include/tile-definitions.h"

void tile_lock_init(){
    int i;

    for(i=0;i<TILE_COUNT;i++){
        pthread_mutex_init(&tile_locks[i], 0x0);
    }
}

int tile_event_activate(int x, int y, void *args){

}

int tile_coord_is_empty(Tile *tile, int x, int y){
    int segment;
    segment = tile_get_segments(x);
    return !((tile->collision[y])->segments[segment] & (0x1<<(x&0x7)));
}

int tile_get_segments(int width){
    return width>>3; 
}

int tile_enable_coord(Tile *tile, int x, int y){
    return tile_toggle_coord(tile, x, y, 0);
}

int tile_disable_coord(Tile *tile, int x, int y){
    return tile_toggle_coord(tile, x, y, 1);
}

int tile_toggle_coord(Tile *tile, int x, int y, int disable){
    int segment;
    // check bounds
    if(y >= tile->height || y < 0 || x >= tile->width || x < 0)
        return 0;

    pthread_mutex_lock(&((tile->collision[y])->lock));
    // check if position is valid
    if(disable && !tile_coord_is_empty(tile, x, y)){
        pthread_mutex_unlock(&((tile->collision[y])->lock));
        return 0;
    }
    // disable position
    segment = tile_get_segments(x);
    if(disable)
        (tile->collision[y])->segments[segment] |= (1<<(x&0x7));
    else
        (tile->collision[y])->segments[segment] &= ~(1<<(x&0x7));
    pthread_mutex_unlock(&((tile->collision[y])->lock));
    return 1;
}

long tile_random_coord(Tile *tile){
    int x, y;

    do{
        // generate random coords
        x = rand() % tile->width;
        y = rand() % tile->height;
    // check that coords are valid
    }while(!tile_coord_is_empty(tile, x, y));
    return (long)x<<32 | (y & 0xffffffff);
}

long tile_spawn_player(int id, Player *player){
    long pos;
    Tile *tile;

    if(id < 0 || id > TILE_COUNT)
        return -1;

    // get tile
    tile = tile_loaders[id]();
    player->tile = tile;

    // get random coordinate
    pos = tile_random_coord(player->tile);

    // assign player their coordinates
    player->x = pos >> 32;
    player->y = pos & 0xffffffff;

    // add player to tile
    tile->players = link_add_first(tile->players, player);

    // set coordinate to be invalid
    if(!tile_disable_coord(tile, player->x, player->y))
        return -1;
    // return coordinates
    return pos;
}

TileRow *tile_row_create(int width){
    int mod, segments, row_size;
    TileRow *row;
    segments = tile_get_segments(width);
    row_size = sizeof(TileRow)+sizeof(char)*segments;
    row = malloc(row_size);
    memset(row, 0, row_size);
    pthread_mutex_init(&(row->lock), 0x0);
    return row;
}

void tile_row_free(int height, TileRow **rows){
    int i;
    for(i=0;i<height;i++){
        pthread_mutex_destroy(&(rows[i]->lock));
        free(rows[i]);
    }
}

Tile *tile_create(int id, int width, int height){
    Tile *tile;
    int i;

    tile = malloc(sizeof(Tile));
    tile->id = id;
    tile->height = height;
    tile->width = width;
    tile->players = 0x0;
    tile->events = 0x0;
    tile->refs = 0;
    pthread_mutex_init(&(tile->lock), 0x0);

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
    if(id > TILE_COUNT || id < 0)
        return 0x0;
    // check if tile is not already initalize (is 0x0)
    pthread_mutex_lock(&tile_locks[id]);
    if(!tiles[id]){
        tiles[id] = tile_loaders[id]();    
    }
    tiles[id]->refs++;
    pthread_mutex_unlock(&tile_locks[id]);
    return tiles[id];
}

void tile_unload(int id){
    if(id > TILE_COUNT || id < 0)
        return;
    if(!tiles[id])
        return;
    pthread_mutex_lock(&tile_locks[id]);
    tiles[id]->refs--;
    if(!tiles[id]->refs){
        tile_free(tiles[id]);
        tiles[id] = 0x0;
    }
    pthread_mutex_unlock(&tile_locks[id]);
}