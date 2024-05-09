
#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>
#include <string.h>
#include "movement.h"
#include "../../maps/include/map.h"

typedef struct player {
    int x;
    int y;
    int max_move;
    struct map *map; // forward inclusion of Map
    char *name;
} Player;

Player *player_create();
void player_free(Player *player);
int player_move(Player *player, int x, int y);

#endif