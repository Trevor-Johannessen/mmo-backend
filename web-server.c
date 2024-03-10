#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "include/buffered_reader.h"

int open_listenfd(char *port);
void *web_server_handle_client(void* fd_arg);
void web_server_get_route(int fd, char *route, char *args);

typedef struct {
    int fd;
    struct sockaddr_storage client_addr;
} ConnInfo;

const int MAX_LINE = 1024; // lines cannot exceed 1023 characters (\0)

int web_server_start(void* port){
    int listen_fd, conn_fd, current_flags;
    pthread_t tid;
    socklen_t client_len;
    struct sockaddr_storage client_addr;
    struct pollfd poll_args;
    ConnInfo *ci;

    if(pthread_detach(pthread_self())){
        fprintf(stderr, "Web server thread detatch failed.\n");
        exit(1);
    }
     
    // Open socket
    listen_fd = open_listenfd((char *)port);
    while(1){
        // Connect to client
        client_len = sizeof(struct sockaddr_storage);
        conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if(conn_fd < 0){
            fprintf(stderr, "Failed to accept.\n");
            continue;
        }
        // Set up client info
        if(!(ci = malloc(sizeof(ConnInfo)))){
            fprintf(stderr, "Failed to allocate client info space.\n");
            close(conn_fd);
            continue;
        }
        ci->fd = conn_fd;
        ci->client_addr = client_addr;
        // Spin up new thread to handle client
        if(pthread_create(&tid, NULL, web_server_handle_client, (void *)ci)){
            fprintf(stderr, "Could not start web client handler thread.\n");
            close(conn_fd);
            continue;
        }
    }
}

void *web_server_handle_client(void* ci){
    int fd, err;
    struct sockaddr_storage client_addr;
    socklen_t client_len;
    char hostname[MAX_LINE], port[MAX_LINE], *route, *args;


    err=0;
    fd = ((ConnInfo *)ci)->fd;
    client_addr = ((ConnInfo *)ci)->client_addr;
    free(ci);
    client_len = sizeof(client_addr);

    if(err = getnameinfo((struct sockaddr *) &client_addr, client_len, hostname, MAX_LINE, port, MAX_LINE, NI_NUMERICSERV)){
        fprintf(stderr, "Could not find hostname and port on fp %d. (Error: %d)\n", fd, err);
        fprintf(stderr, "Error Options: %d EAI_AGAIN, %d EAI_BADFLAGS, %d EAI_FAIL, %d EAI_FAMILY, %d EAI_MEMORY, %d EAI_NONAME, %d EAI_OVERFLOW, %d EAI_SYSTEM\n", EAI_AGAIN, EAI_BADFLAGS, EAI_FAIL, EAI_FAMILY, EAI_MEMORY, EAI_NONAME, EAI_OVERFLOW, EAI_SYSTEM);
        close(fd);
        return NULL;
    }
    fprintf(stdout, "(fd: %d) Accepted connection from %s:%s.\n", fd, hostname, port);
    
    if(pthread_detach(pthread_self())){
        fprintf(stderr, "Auth reciever thread detatch failed.\n");
        close(fd);
        return NULL;
    }
    
    // allocate space for the route and args
    if(!(route = malloc(MAX_LINE))){ // could make this a loop
        fprintf(stderr, "Could not allocate space for route for %s:%s.\n", hostname, port);
        close(fd);
        return NULL;
    }
    if(!(args = malloc(MAX_LINE))){
        fprintf(stderr, "Could not allocate space for args for %s:%s.\n", hostname, port);
        close(fd);
        return NULL;
    }

    // get server route
    web_server_get_route(fd, route, args);
    if(!route){
        fprintf(stderr, "Could not find route for %s:%s.\n", hostname, port);
        close(fd);
        return NULL;
    }
    if(!args){
        fprintf(stderr, "Could not find args for %s:%s.\n", hostname, port);
        close(fd);
        return NULL;
    }
}

void web_server_get_route(int fd, char *route, char *args){

}