#include "include/event-loop.h"

char *temp_get_name(char *id){
    const int BUF_SIZE = 100;
    char *name;
    name = malloc(BUF_SIZE);
    memset(name, 0, BUF_SIZE);
    strcpy(name, "Sam");
    return name;
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
        // Bad login packet, shutdown connection
        return;
    }

    // get code from login packet
    code = packet->data;

    // get player id from awaiting_connections table
    if(!(id = awaiting_connections_table_find(code))){
        // TODO: SEND ERROR PACKET
        return;
    }

    // get name from database (primary key is code)
    name = temp_get_name(id);

    // remove entry from awaiting_connections table
    awaiting_connections_table_remove(code, 0);

    // Set up structs needed for a connection
    player = player_create(name);
    session = session_create(player, code);

    while(1){
        while(poll(&poll_args, 1, -1) <= 0);
        if(!(packet = packet_read(fd))){
            // Bad packet recieved, shutdown connection
        }
    }

}