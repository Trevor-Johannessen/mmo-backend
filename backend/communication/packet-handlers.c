#include "include/packet-handlers.h"

Packet *packet_handle_route(Packet *packet, Session *session){
    switch(packet->opcode){
        case ERROR:
            break;
        case LOGIN_PACKET:
            return packet_handle_login(packet, session);
        case LOGOUT_PACKET:
            return packet_handle_logout(packet, session);
        case SUCCESS_PACKET:
            return packet_handle_success(packet, session);
        case FAILURE_PACKET:
            return packet_handle_failure(packet, session);
        case MOVE_PACKET:
            return packet_handle_move(packet, session);
        default:
            return 0;
    }
}

Packet *packet_handle_login(Packet *packet, Session *session){
    char *id, *code;
    int map_id;
    
    // get code from login packet
    code = packet->data;

    // get player id from awaiting_connections table
    if(!(id = awaiting_connections_table_find(code))){
        packet_errno = INVALID_AWAITING_CONNECTION;
        return 0x0;
    } 

    // remove entry from awaiting_connections table
    awaiting_connections_table_remove(code, 0);

    // get info from database (primary key is code)
    // move this into a Player *db_get_player() function
    session->player->name = db_get_name(id);
    session->player->max_move = 1;
    session->state = ROAMING;
    map_id = 0;

    // spawn player into world
    if(map_spawn_player_random(map_id, session->player) == -1){
        session_destroy(session);
        packet_errno = INVALID_AWAITING_CONNECTION;
        return 0x0;
    }
    return packet_template_success();    
}

Packet *packet_handle_logout(Packet *packet, Session *session){

}

Packet *packet_handle_success(Packet *packet, Session *session){

}

Packet *packet_handle_failure(Packet *packet, Session *session){

}

Packet *packet_handle_move(Packet *packet, Session *session){
    int x, y;

    x = *((int *)(packet->data));
    y = *((int *)(packet->data+4));
    x = ntohl(x);
    y = ntohl(y);
    if(!player_move(session->player, x, y))
        return packet_template_failure();
    return packet_template_success();
}

