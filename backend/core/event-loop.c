#include "include/event-loop.h"

char *temp_get_name(char *id){
    const int BUF_SIZE = 100;
    char *name;
    name = malloc(BUF_SIZE);
    memset(name, 0, BUF_SIZE);
    strcpy(name, "Sam");
    return name;
}

void event_loop_error(int fd, int code){
    Packet *packet;
    if(packet = packet_template_error(code)){
        packet_write(fd, packet);
        packet_free(packet);
    }
    ws_close(fd, 1008);
}

void event_loop_start(int fd){
    Packet *packet;
    Session *session;
    Player *player;
    char *name, *code, *id;
    struct pollfd poll_args;
    
    // set up polling
    poll_args.fd = fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;

    while(poll(&poll_args, 1, -1) <= 0);
    if(!(packet = packet_read(fd))){
        event_loop_error(fd, BAD_LOGIN);
        return;
    }

    // get code from login packet
    code = packet->data;

    // get player id from awaiting_connections table
    if(!(id = awaiting_connections_table_find(code))){
        event_loop_error(fd, INVALID_AWAITING_CONNECTION);
        return;
    }

    // get name from database (primary key is code)
    name = temp_get_name(id);

    // remove entry from awaiting_connections table
    awaiting_connections_table_remove(code, 0);

    // Set up structs needed for a connection
    player = player_create(name);
    session = session_create(player, code, fd);

    while(1){
        while(poll(&poll_args, 1, -1) <= 0);
        if(!(packet = packet_read(session->fd))){
            event_loop_error(session->fd, BAD_PACKET);
        }
    }

}