#include "include/player.h"

PlayerCache *player_cache = 0x0;

void player_cache_init(){
    player_cache = malloc(sizeof(PlayerCache));
    if(!(player_cache->cache = g_hash_table_new_full(g_str_hash, g_str_equal, 0x0, player_cache_destroy_cache))){
        fprintf(stderr, "Could not initalize player cache.\n");
        exit(1);
    }
    pthread_mutex_init(&(player_cache->lock), 0x0);
}

void player_cache_lock(){
    pthread_mutex_lock(&(player_cache->lock));
}

void player_cache_unlock(){
    pthread_mutex_unlock(&(player_cache->lock));
}

void player_cache_destroy(){
    g_hash_table_destroy(player_cache->cache);
}

int player_cache_insert(char *id, Player *player){
    int success = 1;
    player_cache_lock();
    if(!g_hash_table_insert(player_cache->cache, (gpointer)id, (gpointer)player)){
        fprintf(stderr, "Could not insert player: %s.\n", id);
        success = 0;
    }
    player_cache_unlock();
    return success;
}

Player *player_cache_find(char *id){
    Player *player;
    player_cache_lock();
    player = (Player *)g_hash_table_lookup(player_cache->cache, (gconstpointer)id);
    if(!player){
        player_cache_unlock();
        return 0x0;
    }
    player_lock(player);
    player->refs++;
    player_cache_unlock();
    player_unlock(player);
    return player;
}

void player_cache_destroy_cache(gpointer p){
    if(!((Player *)p)->refs)
        return;
    player_free((Player *)p);
}

Player *player_create(){
    Player *player;
    player=malloc(sizeof(*player));
    memset(player, 0, sizeof(*player));
    if(pthread_mutex_init(&player->lock, 0x0)){
        fprintf(stderr, "Player lock init failed.\n");
        free(player);
        return 0x0;
    }
    player->refs=1;
    return player;
}

void player_free(Player *player){
    if(!player)
        return;

    // TODO: Someone can ref the player after this check, add a player lock to ensure mutual access before reffering or unreffing the player

    // aquire player & player cache lock here (does player->refs need to be atomic?)
    player_cache_lock();
    pthread_mutex_lock(&player->lock);

    // reduce refs
    player->refs--;
    if(player->refs > 0){
        player_cache_unlock();
        pthread_mutex_unlock(&player->lock);
        return;
    }

    // Write player to DB
    if(player->modified)
        db_player_write_player(GLOBAL_CONNECTION, player);
    
    // unref map
    if(player->map)
        map_unload(player->map->id);

    // Remove from player cache and release player cache lock
    if(player->id && !g_hash_table_remove(player_cache->cache, (gpointer)(player->id))){
        fprintf(stderr, "Could not remove %s from hashtable.\n", player->id);
    }

    // release locks here
    player_cache_unlock();
    pthread_mutex_destroy(&player->lock);

    // Free player
    if(player->name)
        free(player->name);
    if(player->id)
        free(player->id);
    free(player);
}

Player *player_ref(Player *player){
    pthread_mutex_lock(&player->lock);
    player->refs++;
    pthread_mutex_unlock(&player->lock);
}

void player_lock(Player *player){
    pthread_mutex_lock(&(player->lock));
}

void player_unlock(Player *player){
    pthread_mutex_unlock(&(player->lock));
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