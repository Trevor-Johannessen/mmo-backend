#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "include/buffered-reader.h"
#include "include/web-server.h"

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
    char hostname[MAX_LINE], port[MAX_LINE], *method, *route, *args;
    BufferedReader *br;
    HeaderList *headers;

    err=0;
    br = br_init(((ConnInfo *)ci)->fd, 128);
    client_addr = ((ConnInfo *)ci)->client_addr;
    free(ci);
    client_len = sizeof(client_addr);

    if(err = getnameinfo((struct sockaddr *) &client_addr, client_len, hostname, MAX_LINE, port, MAX_LINE, NI_NUMERICSERV))
        return error("Could not find hostname and port. (Error: %d)\n", hostname, port, br);
    fprintf(stdout, "Accepted connection from %s:%s.\n", hostname, port);
    
    if(pthread_detach(pthread_self()))
        return error("Auth reciever thread detatch failed.", hostname, port, br);
    
    // allocate space for the route and args
    if(!(method = malloc(32)))
        return error("Could not allocate space for method.", hostname, port, br);
    if(!(route = malloc(MAX_LINE)))
        return error("Could not allocate space for route.", hostname, port, br);
    if(!(args = malloc(MAX_LINE)))
        return error("Could not allocate space for args.", hostname, port, br);

    // get server route
    web_server_get_route(br, method, route, args);
    if(br->flags)
        return error("An error occured in web_server_get_route.", hostname, port, br);
    if(!route)
        return error("Could not find route.", hostname, port, br);
    if(!args)
        return error("Could not find args.", hostname, port, br);
    
    // get headers
    headers = web_server_get_headers(br);
    printf("\n\n");
    print_header_list(headers);

    br_destroy(br);
    return NULL;
}

void *error(char* msg, char *hostname, char *port, BufferedReader *br){
    fprintf(stderr, "%s:%s - %s\n", hostname, port, msg);
    br_destroy(br);
    return NULL;
}
// web-server.c:web_server_get_route
void web_server_get_route(BufferedReader *br, char *method, char *route, char *args){
    char* line;
    line = br_read_line(br);
    if(br->flags) // error
        return;
    if(!line) // no \n\r
        return;
    fprintf(stdout, "%s\n", line);
    sscanf(line, "%s %s", method, route);
    fprintf(stdout, "%s - %s\n", method, route);
    free(line);
}

HeaderList *web_server_get_headers(BufferedReader *br){
    char *header_name, *header_line;
    char *header_value;
    HeaderList *head, *cur, *prev;
    
    head = cur = prev = NULL;
    while((header_line = br_read_line(br)) && !br->flags && header_line[0] != '\r'){
        if(!(cur = malloc(sizeof(HeaderList)))){
            destroy_header_list(head);
            return NULL;
        }
        if(prev)
            prev->next_header = cur;
        if(!head)
            head = cur;
        if(!(header_name = (char *)malloc(sizeof(char)*MAX_HEADER))){
            destroy_header_list(head);
            return NULL;
        }
        if(!(header_value =(char *)malloc(sizeof(char)*MAX_LINE))){
            destroy_header_list(head);
            return NULL;
        }
        fprintf(stdout, "%s\n", header_line);
        sscanf(header_line, "%[^:]%*[:] %s", header_name, header_value);
        cur->header_name = header_name;
        cur->header_value = header_value;
        prev = cur;
        free(header_line);
    }
    free(header_line);
    return head;
}

void destroy_header_list(HeaderList *head){
    HeaderList *cur, *hold;
    if(!head)
        return;
    cur=head;
    while(cur->next_header){
        hold = cur;
        cur=cur->next_header;
        free(hold->header_name);
        free(hold->header_value);
        free(hold);
    }
}

void print_header_list(HeaderList *head){
    for(HeaderList *cur=head;cur;cur=cur->next_header)
        fprintf(stdout, "%s - %s\n", cur->header_name, cur->header_value);
}