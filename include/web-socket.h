#include "header-list.h"

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

void create_web_socket(BufferedReader *br, HeaderList *headers);
char *hash_key(char* key);
void print_hash(unsigned char *hash);

#endif