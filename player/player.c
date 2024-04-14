#include "include/player.h"

Player *player_create(char *name){
    Player *player;
    player=malloc(sizeof(player));
    memset(player, 0, sizeof(player));
    player->name = name;
    return player;
}

void player_destroy(Player *player){
    if(!player)
        return;
    if(player->name)
        free(player->name);
    free(player);
}