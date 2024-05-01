#ifndef TILE_H
#define TILE_H

#include <pthread.h>
#include <stdlib.h>
#include "../../structures/include/linked-list.h"
#include "../../player/include/player.h"

typedef struct tile_row {
    pthread_mutex_t lock;
    unsigned char segments[];
} TileRow;

typedef struct tile_event {
    int x; // event position
    int y;
    void *func; // function triggered on event activation
    void *args;
} TileEvent;

typedef struct tile {
    int width;
    int height;
    int id;
    int refs; // unload tile if ref == 0
    pthread_mutex_t lock;
    TileRow **collision;
    char *decor;
    Link *players; // If players have IDs this could be made into a btree for better performance
    Link *events; // This should be a btree based on event-location
} Tile;

extern Tile *tiles[];
extern Tile *(*tile_loaders[])();
extern pthread_mutex_t tile_locks[];

int tile_event_activate(int x, int y, void *args);
int tile_coord_is_empty(Tile *tile, int x, int y);
Tile *tile_load(int id);
void tile_unload(int id);
void tile_initalize();
TileRow *tile_row_create(int width);
Tile *tile_create(int id, int width, int height);
int tile_get_segments(int width);
long tile_spawn_player(int id, Player *player);
void tile_event_free(TileEvent *event);
void tile_row_free(int height, TileRow **rows);
void tile_free(Tile *tile);
long tile_random_coord(Tile *tile);
int tile_disable_coord(Tile *tile, int x, int y);
int tile_enable_coord(Tile *tile, int x, int y);
int tile_toggle_coord(Tile *tile, int x, int y, int disable);
void tile_lock_init();

#endif