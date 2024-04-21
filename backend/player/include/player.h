#include <stdlib.h>
#include <string.h>

#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    short x;
    short y;
    char *name;
} Player;

Player *player_create(char *name);
void player_destroy(Player *player);

#endif