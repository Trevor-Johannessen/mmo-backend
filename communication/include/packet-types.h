
#ifndef PACKETTYPES_H
#define PACKETTYPES_H

#include "packet.h"

// Opcodes
// #define C_NAME code
#define LOGIN_PACKET 1
#define LOGOUT_PACKET 2

Packet *packet_template(unsigned char opcode, long id);
Packet *packet_template_login(long id);
Packet *packet_template_logout(long id);


#endif