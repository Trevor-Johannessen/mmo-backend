#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "buffered-reader.h"
#include "web-socket.h"
#include "header-list.h"

#ifndef WEBSERVER_H
#define WEBSERVER_H

typedef struct {
    int fd;
    struct sockaddr_storage client_addr;
} ConnInfo;

int open_listenfd(char *port);
void *web_server_handle_client(void* fd_arg);
void web_server_get_route(BufferedReader *br, char *method, char *route, char *args);
void *error(char* msg, char *hostname, char *port, BufferedReader *br);

#define MAX_LINE 1024 // lines cannot exceed 1023 characters

#endif