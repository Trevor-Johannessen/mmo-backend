#include <stdio.h>
#include <glib.h>
#include <time.h>
#include <pthread.h>

/*
    Defines the awaiting connections dictionary which maps a token string to an awaiting connection struct.
*/
typedef struct awaiting_connection {
    char *id;
    time_t expiration;
} AwaitingConnection;

void awaiting_connection_destroy();
void awaiting_connections_table_initalize();
int awaiting_connections_table_insert(char* token, char* id);
int awaiting_connections_table_remove(char* token);
int awaiting_connections_table_destroy();
void awaiting_connections_table_print_all();

GHashTable *awaiting_connections_table;
pthread_mutex_t write_lock;

void awaiting_connections_table_initalize(){
    if(pthread_mutex_init(&write_lock, NULL)){
        fprintf(stderr, "Hash table write lock init failed.\n");
        exit(1);
    }
    awaiting_connections_table=0x0;
    if(!(awaiting_connections_table = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, awaiting_connection_destroy))){
        fprintf(stderr, "Could not initalize hash table.\n");
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
    conn->expiration+=5*60; // awaiting connections open for 5 minutes

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
    return success;
}

int awaiting_connections_table_remove(char* token){
    AwaitingConnection *conn;
    int success;

    success=0;
    pthread_mutex_lock(&write_lock);
    if(success && !g_hash_table_remove(awaiting_connections_table, (gconstpointer)token)){
        fprintf(stderr, "Could not remove %s from hashtable.\n", token);
        success=0;
    }
    free(token);
    pthread_mutex_unlock(&write_lock);
    return success;
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