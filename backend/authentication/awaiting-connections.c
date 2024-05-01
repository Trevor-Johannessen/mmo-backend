#include "include/awaiting-connections.h"

GHashTable *awaiting_connections_table = 0x0;
pthread_mutex_t write_lock;
const time_t time_to_live = 5*60;

void awaiting_connections_table_initalize(){
    pthread_t auth_reciever_tid;

    if(pthread_mutex_init(&write_lock, 0x0)){
        fprintf(stderr, "Hash table write lock init failed.\n");
        exit(1);
    }
    awaiting_connections_table=0x0;
    if(!(awaiting_connections_table = g_hash_table_new_full(g_str_hash, g_str_equal, 0x0, awaiting_connection_destroy))){
        fprintf(stderr, "Could not initalize hash table.\n");
        pthread_mutex_destroy(&write_lock);
        exit(1);
    }
    if(pthread_create(&auth_reciever_tid, 0x0, awaiting_connections_table_cleaner, 0x0)){
        fprintf(stderr, "Could not start auth reciever thread.\n");
        pthread_mutex_destroy(&write_lock);
        exit(1);
    }
}

int awaiting_connections_table_insert(char* token, char* id){
    AwaitingConnection *conn;
    gboolean valid_token;
    int success;
    
    // Create new AwaitingConnection
    conn = malloc(sizeof(AwaitingConnection));
    conn->id = id;
    time(&conn->expiration);
    conn->expiration+=time_to_live; // awaiting connections open for 5 minutes

    // Insert connection into hashtable
    pthread_mutex_lock(&write_lock);
    // Check if token is available
    valid_token = g_hash_table_contains(awaiting_connections_table, (gconstpointer)token);
    // Insert into hashtable
    success = 1;
    if(!g_hash_table_insert(awaiting_connections_table, (gpointer)token, (gpointer)conn) || valid_token){
        fprintf(stderr, "Could not insert %s.\n", token);
        success = 0;
    }
    pthread_mutex_unlock(&write_lock);
    // Teardown if failure
    if(!success){
        free(token);
        awaiting_connection_destroy((gpointer)conn);
        free(conn);
    }
    fprintf(stdout, "Inserted: %s\n", token);
    return success;
}

int awaiting_connections_table_remove(char* token, int remove_token){
    AwaitingConnection *conn;
    int success;

    success=0;
    pthread_mutex_lock(&write_lock);
    if(success && !g_hash_table_remove(awaiting_connections_table, (gconstpointer)token)){
        fprintf(stderr, "Could not remove %s from hashtable.\n", token);
        success=0;
    }
    if(remove_token)
        free(token);
    pthread_mutex_unlock(&write_lock);
    return success;
}

char *awaiting_connections_table_find(char *key){
    AwaitingConnection* conn;
    if(!(conn = (AwaitingConnection *)g_hash_table_lookup(awaiting_connections_table, (gconstpointer)key))){
        fprintf(stderr, "Could not find connection with key '%s'.", key);
        return 0x0;
    }
    return conn->id;
}

int awaiting_connections_table_destroy(){
    g_hash_table_destroy(awaiting_connections_table);
}

void awaiting_connection_destroy(gpointer conn){
    free(((AwaitingConnection *)conn)->id);
}

void print(gpointer key, gpointer value, gpointer user_data){
    int *counter = (int *)user_data;
    (*counter)++;
    fprintf(stdout, "%d. %s -> %s\n\n", *counter, (char*)key, ((AwaitingConnection *)value)->id);
}

void awaiting_connections_table_print_all(){
    int counter = 0;
    fprintf(stdout, "Awaiting Connections Content\n");
    g_hash_table_foreach(awaiting_connections_table, print, (gpointer*)&counter);
}

void awaiting_connections_table_check_if_expired(gpointer key, gpointer value, gpointer now){
    AwaitingConnection *conn = (AwaitingConnection *)value;
    if(conn->expiration <= (time_t)now){
        fprintf(stdout, "Removing %s.\n", (char *)key);
        awaiting_connections_table_remove((char *)key, 1);
    }
}

void *awaiting_connections_table_cleaner(void* arg){
    time_t now;

    if(pthread_detach(pthread_self())){
        fprintf(stderr, "Auth reciever thread detatch failed.\n");
        exit(1);
    }
    while(1){
        sleep(time_to_live);
        fprintf(stdout, "Checking awaiting connections for staleness.\n");
        time(&now);
        g_hash_table_foreach(awaiting_connections_table, awaiting_connections_table_check_if_expired, (gpointer)now);
    }
}