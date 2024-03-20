#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <openssl/sha.h>
#include <poll.h>
#include "header-list.h"
#include "buffered-reader.h"
#include "http-packet.h"

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

typedef struct {
    int fin : 1;
    int rsv1 : 1;
    int rsv2 : 1;
    int rsv3: 1;
    int opcode : 4;
    int mask : 1;
    int length : 7;
    int key;
    union {
        short short_len;
        long long_len;   
    } length_ext;
    char data[];
} WS_Frame;

void ws_create(int fd, HeaderList *headers);
void ws_write(int fd, void *string, long len);
void ws_run(int fd);
void ws_echo_server(int fd);
char *hash_key(char* key);
void print_hash(unsigned char *hash);
char* base64_encode_glib(const char* input_string);
WS_Frame *ws_text_frame(char *data);
WS_Frame *ws_bin_frame(void *data, long len);
#endif