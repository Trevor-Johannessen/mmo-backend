#include <stdlib.h>
#include <string.h>

#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    int x;
    int y;
    int tile_id;
    char *name;
} Player;

Player *player_create(char *name);
void player_free(Player *player);

#endif