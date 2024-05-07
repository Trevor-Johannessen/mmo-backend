#include "include/movement.h"

int movement_check_valid(Player *player, int x, int y){
    return player->max_move >= movement_count_distance(player, x, y);
}

int movement_count_distance(Player *player, int x, int y){
    // TODO: Implement A* here
    return 1;
}

int movement_move(Player *player, int x, int y){
    // check if player can move that far
    if(!movement_check_valid)
        return 0;

    // disable space

    // set player to new coords
    player->x = x;
    player->y = y;

    return 1;
}