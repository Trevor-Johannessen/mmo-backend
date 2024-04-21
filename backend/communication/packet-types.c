#include "include/packet-types.h"

Packet *packet_template(unsigned char opcode){
    
}

// Confirms a login has been successful
Packet *packet_template_login(){
    return packet_create(LOGIN_PACKET, 0, 0);
}

// Confirms a logout has been successful
Packet *packet_template_logout(){
    return packet_create(LOGOUT_PACKET, 0, 0);
}