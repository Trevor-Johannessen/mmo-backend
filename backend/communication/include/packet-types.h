
#ifndef PACKETTYPES_H
#define PACKETTYPES_H

#include "packet.h"
#include "errors.h"

// Opcodes
// #define C_NAME code
#define ERROR 0
#define LOGIN_PACKET 1
#define LOGOUT_PACKET 2
#define SUCCESS_PACKET 3
#define FAILURE_PACKET 4
#define MOVE_PACKET 5

Packet *packet_template(unsigned char opcode);
Packet *packet_template_error(int code);
Packet *packet_template_login();
Packet *packet_template_logout();
Packet *packet_template_success();
Packet *packet_template_failure();
Packet *packet_template_move();


#endif