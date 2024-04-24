#include <stdlib.h>
#include <string.h>

#ifndef PLAYER_H
#define PLAYER_H


typedef struct {
    int x;
    int y;
    struct tile *tile; // forward inclusion of Tile
    char *name;
} Player;

Player *player_create();
void player_free(Player *player);

#endif