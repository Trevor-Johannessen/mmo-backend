#ifndef AWAITINGCONNECTION_H
#define AWAITINGCONNECTION_H

#include <stdio.h>
#include <glib.h>
#include <time.h>
#include <pthread.h>
//#include <windows.h> // sleep for windows
#include <unistd.h> // sleep for linux

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
int awaiting_connections_table_remove(char* token, int remove_token);
int awaiting_connections_table_destroy();
void awaiting_connections_table_print_all();
void awaiting_connections_table_check_if_expired(gpointer key, gpointer value, gpointer now);
void *awaiting_connections_table_cleaner(void* arg);
char *awaiting_connections_table_find(char *key);

extern GHashTable *awaiting_connections_table;

#endif