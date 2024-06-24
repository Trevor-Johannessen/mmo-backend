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
    if(player->id)
        free(player->id);
    free(player);
}

int player_move(Player *player, int x, int y){
    Packet *packet;

    // check if player can move that far
    if(!movement_check_valid(player->max_move, player->x, player->y, x, y))
        return 0;

    // enable old coord
    if(!map_enable_coord(player->map, player->x, player->y))
        return 0;

    // disable space
    if(!map_disable_coord(player->map, x, y))
        return 0;

    // set player to new coords
    player->x = x;
    player->y = y;

    // update other players on new position
    packet = packet_template_update_position(0, player->id, x, y);
    map_send_packet(player->map, packet, 0x0);

    // cleanup
    packet_free(packet);
    
    return 1;
}

void player_print(Player *player){
    printf("%s: id=%s, map=%d, x=%d, y=%d, max_move=%d\n", player->name, player->id, player->map->id, player->x, player->y, player->max_move);
}