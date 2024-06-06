#include "include/db.h"

char *db_get_name(char *id){
    const int BUF_SIZE = 100;
    char *name;
    name = malloc(BUF_SIZE);
    memset(name, 0, BUF_SIZE);
    strcpy(name, "Sam");
    return name;
}


MongoConnection *db_connect(){
    MongoConnection *conn;
    char *connection_string;
    bson_t *command;
    int ok;
    bson_error_t error = {0};
    bson_t reply = BSON_INITIALIZER;

    conn = malloc(sizeof(MongoConnection));
    memset(conn, 0, sizeof(*conn));
    command = 0x0;

    // get client
    connection_string = getenv("MMO_MONGO_CONNECTION_STRING");
    conn->client = mongoc_client_new(connection_string);
    free(connection_string);
    if(!conn->client)
        goto mongo_connection_create_cleanup;

    // get api
    conn->api = mongoc_server_api_new(MONGOC_SERVER_API_V1);
    if(!conn->api)
        goto mongo_connection_create_cleanup;

    // get database
    conn->database = mongoc_client_get_database(conn->client, "MMO");
    if(!conn->database)
        goto mongo_connection_create_cleanup;

    // ping db
    command = BCON_NEW("ping", BCON_INT32(1));
    ok = mongoc_database_command_simple(conn->database, command, NULL, &reply, &error);
    if (!ok)
        goto mongo_connection_create_cleanup;
    bson_destroy(&reply);

    // cleanup
    if(0){
        mongo_connection_create_cleanup:
        mongoc_database_destroy(conn->database);
        mongoc_server_api_destroy(conn->api);
        mongoc_client_destroy(conn->client);
        free(conn);
        conn = 0x0;
    }

    return conn;
}