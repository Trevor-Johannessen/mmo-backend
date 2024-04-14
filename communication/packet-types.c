#include "include/packet-types.h"

Packet *packet_template(unsigned char opcode, long id){
    
}

// Confirms a login has been successful
Packet *packet_template_login(long id){
    return packet_create(LOGIN_PACKET, id, 0, 0);
}

// Confirms a logout has been successful
Packet *packet_template_logout(long id){
    return packet_create(LOGOUT_PACKET, id, 0, 0);
}