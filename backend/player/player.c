#include "include/player.h"

GHashTable *player_cache = 0x0;

void player_cache_init(){
    if(!(player_cache = g_hash_table_new_full(g_str_hash, g_str_equal, 0x0, player_cache_destroy_cache))){
        fprintf(stderr, "Could not initalize player cache.\n");
        exit(1);
    }
}

void player_cache_destroy(){
    g_hash_table_destroy(player_cache);
}

int player_cache_insert(char *id, Player *player){
    int success = 1;

    if(!g_hash_table_insert(player_cache, (gpointer)id, (gpointer)player)){
        fprintf(stderr, "Could not insert player: %s.\n", id);
        success = 0;
    }

    return success;
}

Player *player_cache_find(char *id){
    Player *player;

    if(!(player = (Player *)g_hash_table_lookup(player_cache, (gconstpointer)id)))
        return 0x0;
    atomic_fetch_add(&(player->refs), 1);
    return player;
}

void player_cache_destroy_cache(gpointer p){
    player_free((Player *)p);
}

Player *player_create(){
    Player *player;
    player=malloc(sizeof(*player));
    memset(player, 0, sizeof(*player));
    return player;
}

void player_free(Player *player){
    if(!player)
        return;

    // TODO: Someone can ref the player after this check, add a player lock to ensure mutual access before reffering or unreffing the player

    // aquire lock here (does player->refs need to be atomic?)

    atomic_fetch_add(&(player->refs), -1);
    if(player->refs > 0)
        return;



    // Write player to DB
    if(player->modified)
        db_player_write_player(GLOBAL_CONNECTION, player);
    
    // unref map
    if(player->map)
        map_unload(player->map->id);

    // TODO: Why don't we remove from the player cache????
    // if(!g_hash_table_remove(player_cache, (gpointer)(player->id))){
    //     fprintf(stderr, "Could not remove %s from hashtable.\n", player->id);
    // }

    // release lock here

    // Free player
    if(player->name)
        free(player->name);
    if(player->id)
        free(player->id);
    free(player);
}

int player_move(Player *player, int x, int y, MoveArgs args){
    Packet *packet;
    Map *old_map;

    // update player's map
    old_map = player->map;
    if(args.map){
        map_remove_player(player->map, player);
        map_add_player(args.map, player);
    }

    // check if player can move that far
    if(!args.map && !movement_check_valid(player->max_move, player->x, player->y, x, y))
        return 0;

    // check if coord is in bounds
    if(!map_check_bounds(player->map, x, y))
        return 0;

    // disable space
    if(!map_disable_coord(player->map, x, y) && !args.overlap)
        return 0;

    // enable old coord
    if(old_map && !map_enable_coord(old_map, player->x, player->y))
        return 0;

    // set player to new coords
    player->x = x;
    player->y = y;

    // trigger any events
    if(!args.suppress_events)
        map_event_activate(x, y, player->map, player);

    // update other players on new position
    packet = packet_template_update_position(player->id, player->x, player->y);
    map_send_packet(player->map, packet, 0x0);

    // cleanup
    packet_free(packet);
    
    return 1;
}

void player_change_name(Player *player, char *name){
    char *new_name;
    new_name = malloc(sizeof(char) * strlen(name)+1);
    strcpy(new_name, name);
    free(player->name);
    player->name = new_name;
}

void player_print(Player *player){
    printf("%s: id=%s, map=%d, x=%d, y=%d, max_move=%d\n", player->name, player->id, player->map->id, player->x, player->y, player->max_move);
}