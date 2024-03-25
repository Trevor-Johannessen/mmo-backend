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

/*
    auth-reciever.c
    This file contains the auth reciever which recieves tokens from the auth server to store in the awaiting connections dictionary
    and sets up listening web servers for the client to connect to. These servers will then be upgraded into websockets.
*/

#define ENABLE_DICT 1
#define DEBUG_DICT 1

const int TOKEN_SIZE = 255;
const int REPS = 5;

int open_listenfd(char* port);
int readSocket(int fd, char* token);
void awaiting_connections_table_initalize();
int awaiting_connections_table_insert(char* token, char* id);
int awaiting_connections_table_remove(char* token);
int awaiting_connections_table_destroy();
void awaiting_connections_table_print_all();

void *start_auth_reciever(void* port){
    int listen_fd, conn_fd, current_flags;
    int id_size;
    socklen_t client_len;
    struct sockaddr_storage client_addr;
    char *token, *id;
    struct pollfd poll_args;

    if(pthread_detach(pthread_self())){
        fprintf(stderr, "Auth reciever thread detatch failed.\n");
        exit(1);
    }
        
    // Initalize variables
    id_size = strlen("111111111111111111111");
    #if ENABLE_DICT == 1
        awaiting_connections_table_initalize();
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
    fprintf(stdout, "Connected.\n");
    
    // Set poll args
    poll_args.fd = conn_fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;

    for(int i=0; i<REPS; i++){
        id = malloc(sizeof(char) * (id_size+1));
        token = malloc(sizeof(char) * (TOKEN_SIZE+1));
        memset(id, 0, sizeof(char) * (id_size+1));
        memset(token, 0, sizeof(char) * (TOKEN_SIZE+1));

        // Get token
        fprintf(stdout, "Polling.\n");
        while(poll(&poll_args, 1, -1) <= 0);
        fprintf(stdout, "There is data ready to be read.\n");
        if(!readSocket(conn_fd, token)){
            fprintf(stderr, "Could not read token\n");
            exit(1);
        }
        fprintf(stdout, "Token: %s\n", token);
        fprintf(stdout, "Finished reading.\n");

        // Get google ID
        fprintf(stdout, "Polling.\n");
        while(poll(&poll_args, 1, -1) <= 0);
        fprintf(stdout, "There is data ready to be read.\n");
        if(!readSocket(conn_fd, id)){
            fprintf(stderr, "Could not read ID\n");
            exit(1);
        }
        fprintf(stdout, "ID: %s\n", id);
        fprintf(stdout, "Finished reading.\n");

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
    getaddrinfo(NULL, port, &hints, &listp);

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

int readSocket(int fd, char* token){
    int total_read=0;
    int amount_read;
    while((amount_read = read(fd, &token[total_read], TOKEN_SIZE-total_read)) > 0)
        total_read+=amount_read;
    return total_read;
}

