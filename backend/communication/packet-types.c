#include "include/packet-types.h"

Packet *packet_template(unsigned char opcode){
    
}

// INCOMING PACKETS

// Confirms a login has been successful
Packet *packet_template_login(){
    return packet_create(LOGIN_PACKET, 0, 0);
}

// Confirms a logout has been successful
Packet *packet_template_logout(){
    return packet_create(LOGOUT_PACKET, 0, 0);
}

// Moves a player to a given coordiante
Packet *packet_template_move(){
    void *body;
    int size = sizeof(int)*2;
    if(!(body = malloc(size)))
        return 0;
    packet_create(MOVE_PACKET, body, size);
}

// OUTGOING PACKETS

Packet *packet_template_success(){
    return packet_create(SUCCESS_PACKET, 0, 0);
}

Packet *packet_template_failure(){
    return packet_create(FAILURE_PACKET, 0, 0);
}

Packet *packet_template_bad_state(){
    return packet_create(BAD_STATE, 0, 0);
}

Packet *packet_template_error(int code){
    char *msg;
    if(code > (error_length-1))
        code = 0;
    msg = error_strings[code];
    return packet_create(ERROR, msg, strlen(msg));
}

Packet *packet_template_update_position(char *id, int x, int y){
    int size;
    char *body;

    if(!id)
        return 0x0;

    size = sizeof(int)*2 + strlen(id);
    body = malloc(size);

    ((int *)body)[0] = x;
    ((int *)body)[1] = y;
    strcpy(body+sizeof(int)*2, id);

    return packet_create(MOVED_PACKET, body, size);
}