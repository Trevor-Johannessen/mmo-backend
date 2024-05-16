#include "include/packet-types.h"

Packet *packet_template(unsigned char opcode){
    
}

Packet *packet_template_error(int code){
    char *msg;
    if(code > (error_length-1))
        code = 0;
    msg = error_strings[code];
    return packet_create(ERROR, msg, strlen(msg));
}

// Confirms a login has been successful
Packet *packet_template_login(){
    return packet_create(LOGIN_PACKET, 0, 0);
}

// Confirms a logout has been successful
Packet *packet_template_logout(){
    return packet_create(LOGOUT_PACKET, 0, 0);
}

Packet *packet_template_success(){
    return packet_create(SUCCESS_PACKET, 0, 0);
}

Packet *packet_template_failure(){
    return packet_create(FAILURE_PACKET, 0, 0);
}

// Moves a player to a given coordiante
Packet *packet_template_move(){
    void *body;
    int size = sizeof(int)*2;
    if(!(body = malloc(size)))
        return 0;
    packet_create(MOVE_PACKET, body, size);
}