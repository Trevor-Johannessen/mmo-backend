#include "include/session.h"

StateArray **session_valid_packets;

Session *session_create(int fd){
    Session *session;
    
    session = malloc(sizeof(Session));
    memset(session, 0, sizeof(Session));
    session->player = 0x0;
    session->fd = fd;
    return session;
}

void session_destroy(Session *session){
    if(!session)
        return;
    if(session->player)
        player_free(session->player);
    free(session);
}