#ifndef DB_H
#define DB_H

#include <string.h>
#include <stdlib.h>
#include <mongoc/mongoc.h>

typedef struct mongo_connection {
    mongoc_client_t *client;
    mongoc_server_api_t *api;
    mongoc_database_t *database;
} MongoConnection;

MongoConnection *db_connect();
char *bson_get_string(bson_iter_t *iter);
char *db_get_name(char *id);
void db_free(MongoConnection *conn);


#endif