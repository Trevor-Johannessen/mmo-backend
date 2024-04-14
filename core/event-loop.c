#include "include/event-loop.h"

void event_loop_start(int fd){
    Packet *packet;
    Session *session;
    Player *player;
    char *name;
    struct pollfd poll_args;
    
    // set up polling
    poll_args.fd = fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;

    while(poll(&poll_args, 1, -1) <= 0);
    if(!(packet = packet_read(fd))){
        // Bad login packet, shutdown connection
    }
    return;

    // get code from login packet

    // get name from database (primary key is code)

    // Set up structs needed for a connection
    player = player_create(name);
    session = session_create(player, 0); // replace 0 with session id

    while(1){
        if(!(packet = packet_read(fd))){
            // Bad packet recieved, shutdown connection
        }
    }

}