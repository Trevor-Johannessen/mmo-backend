
#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <string.h>
#include "movement.h"
#include "../../maps/include/map.h"
#include "../../communication/include/packet-types.h"
#include "session.h"

typedef struct player {
    int x;
    int y;
    int max_move;
    struct map *map;
    struct session *session;
    char *name; // names must be UNIQUE
} Player;

Player *player_create();
void player_free(Player *player);
int player_move(Player *player, int x, int y);

#endif