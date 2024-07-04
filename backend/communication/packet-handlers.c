#include "include/packet-handlers.h"

Packet *packet_handle_route(Packet *packet, Session *session){
    switch(packet->opcode){
        case ERROR:
            break;
        case LOGIN_PACKET:
            return packet_handle_login(packet, session);
        case LOGOUT_PACKET:
            return packet_handle_logout(packet, session);
        case MOVE_PACKET:
            return packet_handle_move(packet, session);
        case INSPECT_PLAYER_PACKET:
            return packet_handle_inspect_player(packet, session);
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

    // get info from database (primary key is code)
    if(!(session->player = db_player_get_player(session->conn, id))){
        return packet_template_failure(packet->id);
    }
    session->player->session = session;
    // session->player->name = db_get_name(id);
    // session->player->max_move = 1;
    session->state = ROAMING;
    map_id = 0;

    // remove entry from awaiting_connections table
    awaiting_connections_table_remove(code, 0);

    // spawn player into world
    if(map_spawn_player(map_id, session->player, session->player->x, session->player->y) == -1){
        session_destroy(session);
        packet_errno = INVALID_AWAITING_CONNECTION;
        return 0x0;
    }
    return packet_template_success(packet->id);    
}

Packet *packet_handle_logout(Packet *packet, Session *session){

}

Packet *packet_handle_move(Packet *packet, Session *session){
    int x, y;

    x = *((int *)(packet->data));
    y = *((int *)(packet->data+4));
    if(!player_move(session->player, x, y))
        return packet_template_failure(packet->id);
    //return packet_template_success(packet->id);
    return 0x0;
}

Packet *packet_handle_inspect_player(Packet *packet, Session *session){
    char *id;
    Player *player;
    Packet *out;

    id = malloc(packet->length+1);
    strncpy(id, packet->data, packet->length);
    id[packet->length] = '\0';
    if(!(player = db_player_get_player(session->conn, id)))
        out = packet_template_failure(packet->id);
    else
        out = packet_template_player(packet->id, player->name, player->x, player->y);
    free(id);
    return out;
}
