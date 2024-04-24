#include "include/player.h"

Player *player_create(){
    Player *player;
    player=malloc(sizeof(player));
    memset(player, 0, sizeof(player));
    return player;
}

void player_free(Player *player){
    if(!player)
        return;
    if(player->name)
        free(player->name);
    free(player);
}