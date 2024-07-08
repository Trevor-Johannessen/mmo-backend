
#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdatomic.h>
#include "movement.h"
#include "session.h"
#include "../../maps/include/map.h"
#include "../../communication/include/packet-types.h"
#include "../../db/include/db-player.h"

typedef struct player {
    int x;
    int y;
    short modified;
    short max_move;
    atomic_int refs;
    struct map *map;
    struct session *session;
    char *name; 
    char *id;
} Player;

extern MongoConnection *GLOBAL_CONNECTION;

void player_cache_init();
void player_cache_destroy();
void player_cache_destroy_cache(gpointer p);
int player_cache_insert(char *id, Player *player);
Player *player_cache_find(char *id);

Player *player_create();
void player_free(Player *player);
int player_move(Player *player, int x, int y, int suppress_events);
void player_print(Player *player);
void player_change_name(Player *player, char *name);

extern GHashTable *player_cache;

#endif