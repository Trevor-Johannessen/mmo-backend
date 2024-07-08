#ifndef MAP_H
#define MAP_H

#include <pthread.h>
#include <stdlib.h>
#include "../../structures/include/linked-list.h"
#include "../../player/include/player.h"
#include "../../player/include/session.h"
#include "../../communication/include/packet.h"
#include "map-events.h"

struct player;

typedef struct map_row {
    pthread_mutex_t lock;
    unsigned char segments[];
} TileRow;

typedef struct map_event {
    int x; // event position
    int y;
    void (*func)(MapEventArgs *);
} MapEvent;

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
extern Map *(*map_loaders[])();
extern pthread_mutex_t map_locks[];

int map_coord_is_walkable(Map *map, int x, int y);
Map *map_load(int id);
void map_unload(int id);
void map_initalize();
TileRow *map_row_create(int width);
Map *map_create(int id, int width, int height);
int map_get_segments(int width);
int map_spawn_player(int id, struct player *player, int x, int y);
long map_spawn_player_random(int id, struct player *player);
void map_send_map_packet(Map *map, Session *session);
void map_add_event(Map *map, MapEvent *event);
MapEvent *map_event_create(int x, int y, void (*func)(MapEventArgs *));
void map_event_free(MapEvent *event);
void map_event_activate(int x, int y, Map *map, struct player *player);
MapEventArgs *map_event_args_create(Map *map, struct player *player);
void map_event_args_free(MapEventArgs *args);
void map_row_free(int height, TileRow **rows);
void map_free(Map *map);
long map_random_coord(Map *map);
int map_disable_coord(Map *map, int x, int y);
int map_enable_coord(Map *map, int x, int y);
int map_toggle_coord(Map *map, int x, int y, int disable);
void map_send_packet(Map *map, Packet *packet, struct player *exception);
void map_lock_init();

#endif