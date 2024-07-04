#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>
#include <fcntl.h>
#include <pthread.h>
#include "include/awaiting-connections.h"

/*
    auth-reciever.c
    This file contains the auth reciever which recieves tokens from the auth server to store in the awaiting connections dictionary
    and sets up listening web servers for the client to connect to. These servers will then be upgraded into websockets.
*/

#define ENABLE_DICT 1
#define DEBUG_DICT 1

const int TOKEN_SIZE = 255;
const int ID_SIZE = 21;
const int REPS = 5;

int open_listenfd(char* port);
int read_socket(int fd, char* token, int read_size);

void *start_auth_reciever(void* port){
    int listen_fd, conn_fd, current_flags;
    socklen_t client_len;
    struct sockaddr_storage client_addr;
    char *token, *id;
    struct pollfd poll_args;

    if(pthread_detach(pthread_self())){
        fprintf(stderr, "Auth reciever thread detatch failed.\n");
        exit(1);
    }
        
    // Initalize variables
    #if ENABLE_DICT == 1
        awaiting_connections_table_init();
    #endif
    // Open socket
    listen_fd = open_listenfd((char *)port);
    client_len = sizeof(struct sockaddr_storage);
    conn_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
    if(conn_fd < 0){
        fprintf(stderr, "Failed to accept.\n");
        exit(1);
    }
    current_flags = fcntl(conn_fd, F_GETFL, 0x0);
    fcntl(conn_fd, F_SETFL, current_flags | O_NONBLOCK);
    
    // Set poll args
    poll_args.fd = conn_fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;

    while(1){
        id = malloc(sizeof(char) * (ID_SIZE+1));
        token = malloc(sizeof(char) * (TOKEN_SIZE+1));
        memset(id, 0, sizeof(char) * (ID_SIZE+1));
        memset(token, 0, sizeof(char) * (TOKEN_SIZE+1));

        // Get token
        while(poll(&poll_args, 1, -1) <= 0);
        if(!read_socket(conn_fd, token, TOKEN_SIZE)){
            fprintf(stderr, "Could not read token\n");
            exit(1);
        }

        // Get google ID
        while(poll(&poll_args, 1, -1) <= 0);
        if(!read_socket(conn_fd, id, ID_SIZE)){
            exit(1);
        }

        // Register token
        #if ENABLE_DICT == 1
            awaiting_connections_table_insert(token, id);
        #endif
        
        #if ENABLE_DICT == 1 && DEBUG_DICT == 1
            awaiting_connections_table_print_all();
        #endif
    }

    // Close auth socket
    close(conn_fd);
    #if ENABLE_DICT == 1
        awaiting_connections_table_destroy();
    #endif
}

int open_listenfd(char* port){
    struct addrinfo hints, *listp, *p;
    int listen_fd;
    int optval=1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // Set to be a listening socket
    hints.ai_flags |= AI_NUMERICSERV; // Only return sockets associated with ports and not services
    hints.ai_flags |= AI_ADDRCONFIG; // Get appropriate IPs
    getaddrinfo(0x0, port, &hints, &listp);

    for(p = listp; p; p = p->ai_next){
        if((listen_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;

        setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

        if (!bind(listen_fd, p->ai_addr, p->ai_addrlen)) // success
            break;
        close(listen_fd);
    }
    freeaddrinfo(listp);
    
    if(!p)
        return -1;
    if(listen(listen_fd, 1024) < 0){
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}

int read_socket(int fd, char* token, int read_size){
    int total_read=0;
    int amount_read;
    while((amount_read = read(fd, &token[total_read], read_size-total_read)) > 0)
        total_read+=amount_read;
    return total_read;
}

