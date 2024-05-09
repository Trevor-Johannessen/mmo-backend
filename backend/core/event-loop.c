#include "include/event-loop.h" 
#include "../maps/include/map.h"

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
/*
    I don't know how I feel about this. Should we have a persistent thread running for every active connection, or a thread pool to
    poll all open file descriptors? I think this can be implemented at a later date since it would still derive from the logic here.
*/
void event_loop_start(int fd){
    Packet *packet;
    Session *session;
    Player *player;
    int map_id;
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

    // remove entry from awaiting_connections table
    awaiting_connections_table_remove(code, 0);

    // Set up structs needed for a connection
    player = player_create();
    session = session_create(player, code, fd);


    // get info from database (primary key is code)
    player->name = temp_get_name(id);
    player->max_move = 1;
    map_id = 0;

    // spawn player into world
    if(map_spawn_player_random(map_id, player) == -1){
        session_destroy(session);
        event_loop_error(fd, INVALID_AWAITING_CONNECTION);
        return;
    }

    while(1){
        while(poll(&poll_args, 1, -1) <= 0);
        if(!(packet = packet_read(session->fd))){
            event_loop_error(session->fd, BAD_PACKET);
        }
    }

}