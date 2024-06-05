#include "include/event-loop.h" 
#include "../maps/include/map.h"

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
    Packet *packet, *response;
    Session *session;
    Player *player;
    struct pollfd poll_args;
    
    // Set up structs needed for a connection
    player = player_create();
    session = session_create(player, fd);

    // set up polling
    poll_args.fd = fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;

    while(1){
        packet = 0x0;
        response = 0x0;

        fprintf(stdout, "State = %d\n", session->state);

        while(poll(&poll_args, 1, -1) <= 0);
        if(!(packet = packet_read(session->fd))){
            event_loop_error(session->fd, BAD_PACKET);
        }

        // check if packet is valid for session state
        if(!session_verify_packet(session, packet)){
            response = packet_template_bad_state();
            packet_write(session->fd, response);
            goto cleanup;
        }

        // check if packet errored
        if(packet_errno)
            event_loop_error(session->fd, packet_errno);

        // execute packets associated function
        response = packet_handle_route(packet, session);

        // send response packet
        if(response)
            packet_write(session->fd, response);

        // clean up
        cleanup:
        if(packet)
            packet_free(packet);
    }

}