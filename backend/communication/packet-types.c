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