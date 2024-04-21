
#ifndef PACKETTYPES_H
#define PACKETTYPES_H

#include "packet.h"
#include "errors.h"

// Opcodes
// #define C_NAME code
#define ERROR 0
#define LOGIN_PACKET 1
#define LOGOUT_PACKET 2

Packet *packet_template(unsigned char opcode);
Packet *packet_template_error(int code);
Packet *packet_template_login();
Packet *packet_template_logout();


#endif