#include "include/player.h"

Player *player_create(){
    Player *player;
    player=malloc(sizeof(*player));
    memset(player, 0, sizeof(*player));
    return player;
}

void player_free(Player *player){
    if(!player)
        return;
    if(player->name)
        free(player->name);
    free(player);
}

int player_move(Player *player, int x, int y){
    // check if player can move that far
    if(!movement_check_valid(player->max_move, player->x, player->y, x ,y))
        return 0;

    // disable space
    if(!map_disable_coord(player->map, x, y))
        return 0;

    // enable old coord
    if(!map_enable_coord(player->map, player->x, player->y))
        return 0;

    // set player to new coords
    player->x = x;
    player->y = y;

    return 1;
}