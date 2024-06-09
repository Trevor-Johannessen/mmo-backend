#include "include/db-player.h"

Player *db_player_get_player(MongoConnection *conn, char *id){
    const bson_t *result;
    bson_t *query;
    bson_error_t error;
    bson_iter_t iter;
    mongoc_cursor_t *cursor;
    mongoc_collection_t *collection;
    Player *player;

    // Initalize
    player = 0x0;
    query = bson_new();
    BSON_APPEND_UTF8(query, "id", id);
    if(!(collection = mongoc_database_get_collection(conn->database, "Playerdata")))
        goto db_get_player_cleanup;
    
    // Find the document
    cursor = mongoc_collection_find_with_opts(collection, query, 0x0, 0x0);
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

    // Cleanup
    db_get_player_cleanup:
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
    return player;
}