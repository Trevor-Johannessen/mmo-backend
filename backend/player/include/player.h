#include <stdlib.h>
#include <string.h>

#ifndef PLAYER_H
#define PLAYER_H


typedef struct {
    int x;
    int y;
    int max_move;
    struct map *map; // forward inclusion of Map
    char *name;
} Player;

Player *player_create();
void player_free(Player *player);

#endif