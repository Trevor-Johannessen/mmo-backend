#include "include/db-player.h"

Player *db_player_get_player(MongoConnection *conn, char *id){
    const bson_t *result;
    bson_t *query;
    bson_error_t error;
    bson_iter_t iter;
    mongoc_cursor_t *cursor;
    Player *player;
    int map_id;

    // Check if player is cached
    // if(player = player_cache_find(id))
    //     return player;

    // Initalize
    player = 0x0;
    query = bson_new();
    BSON_APPEND_UTF8(query, "id", id);
    
    // Find the document
    cursor = mongoc_collection_find_with_opts(conn->playerdata, query, 0x0, 0x0);
    if(!mongoc_cursor_next(cursor, &result))
        goto db_get_player_cleanup;
    player = player_create();

    // Extract information from document
    bson_iter_init(&iter, result);
    while(bson_iter_next(&iter)){
        const char *key = bson_iter_key(&iter);
        if(!strcmp(key, "id"))
            player->id = bson_get_string(&iter);
        else if(!strcmp(key, "name"))
            player->name = bson_get_string(&iter);
        else if(!strcmp(key, "map"))
            player->map = map_load(bson_iter_int32(&iter));
        else if(!strcmp(key, "x"))
            player->x = bson_iter_int32(&iter);
        else if(!strcmp(key, "y"))
            player->y = bson_iter_int32(&iter);
        else if(!strcmp(key, "max_move"))
            player->max_move = bson_iter_int32(&iter);
    }

    // Insert player into cache
    //player_cache_insert(id, player);
    atomic_fetch_add(&(player->refs), 1);

    // Cleanup
    db_get_player_cleanup:
    mongoc_cursor_destroy(cursor);

    return player;
}

void db_player_write_player(MongoConnection *conn, Player *player){
    mongoc_find_and_modify_opts_t *opts;
    bson_t *update;
    bson_t reply;
    bson_error_t error;
    bson_t query = BSON_INITIALIZER;
    int success;
    
    // Construct query to find player
    BSON_APPEND_UTF8(&query, "id", player->id);

    // Construct update query
    update = BCON_NEW ("$set", 
    "{", 
        "name", BCON_UTF8(player->name), 
        "map", BCON_INT32(player->map->id), 
        "x", BCON_INT32(player->x),
        "y", BCON_INT32(player->y),
        "max_move", BCON_INT32(player->max_move), 
    "}");
    opts = mongoc_find_and_modify_opts_new ();
    mongoc_find_and_modify_opts_set_update (opts, update);

    // Perform update
    success = mongoc_collection_find_and_modify_with_opts (conn->playerdata, &query, opts, &reply, &error);
    if (!success) {
        fprintf(stderr, "Could not update used %s. Error: %s\n", player->id, error.message);
    }

    // Cleanup
    bson_destroy(&reply);
    bson_destroy(update);
    bson_destroy(&query);
    mongoc_find_and_modify_opts_destroy(opts);
}