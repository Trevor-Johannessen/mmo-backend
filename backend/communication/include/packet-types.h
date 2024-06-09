
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
#define MOVED_PACKET 6
#define BAD_STATE 7

// INCOMING PACKETS
Packet *packet_template(unsigned char opcode);
Packet *packet_template_login();
Packet *packet_template_logout();
Packet *packet_template_move();

// OUTGOING PACKETS
Packet *packet_template_error(int code);
Packet *packet_template_success();
Packet *packet_template_failure();
Packet *packet_template_bad_state();
Packet *packet_template_update_position(char *id, int x, int y);

#endif