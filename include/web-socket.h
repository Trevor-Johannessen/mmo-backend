#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <openssl/sha.h>
#include <poll.h>
#include <arpa/inet.h>
#include "header-list.h"
#include "buffered-reader.h"
#include "http-packet.h"

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

typedef struct {
    unsigned int fin : 1;
    unsigned int rsv1 : 1;
    unsigned int rsv2 : 1;
    unsigned int rsv3: 1;
    unsigned int opcode : 4;
    unsigned int mask : 1;
    unsigned int length : 7;
    int key;
    union {
        short short_len;
        long long_len;   
    } length_ext;
    char *data;
} WS_Frame;

void ws_create(int fd, HeaderList *headers);
void ws_write(int fd, void *string, long len);
void ws_write_frame(int fd, WS_Frame *frame);
unsigned short ws_transcribe_headers(WS_Frame *frame);
void ws_run(int fd);
void ws_echo_server(int fd);
char *hash_key(char* key);
void print_hash(unsigned char *hash);
char* base64_encode_glib(const char* input_string);
WS_Frame *ws_text_frame(char *data);
WS_Frame *ws_bin_frame(void *data, long len);
WS_Frame *ws_close_frame(short *status_code);
void ws_apply_key(int key, long len, char *data);
#endif