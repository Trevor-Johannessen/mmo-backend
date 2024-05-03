#ifndef MAP_H
#define MAP_H

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

typedef struct map {
    int width;
    int height;
    int id;
    int refs; // unload map if ref == 0
    pthread_mutex_t lock;
    TileRow **collision;
    char *decor;
    Link *players; // If players have IDs this could be made into a btree for better performance
    Link *events; // This should be a btree based on event-location
} Map;

typedef struct {
    int x;
    int y;
} Coordinate;

extern Map *tiles[];
extern Map *(*tile_loaders[])();
extern pthread_mutex_t tile_locks[];

int tile_event_activate(int x, int y, void *args);
int tile_coord_is_empty(Map *map, int x, int y);
Map *tile_load(int id);
void tile_unload(int id);
void tile_initalize();
TileRow *tile_row_create(int width);
Map *tile_create(int id, int width, int height);
int tile_get_segments(int width);
long tile_spawn_player(int id, Player *player);
void tile_event_free(TileEvent *event);
void tile_row_free(int height, TileRow **rows);
void tile_free(Map *map);
long tile_random_coord(Map *map);
int tile_disable_coord(Map *map, int x, int y);
int tile_enable_coord(Map *map, int x, int y);
int tile_toggle_coord(Map *map, int x, int y, int disable);
void tile_lock_init();

#endif