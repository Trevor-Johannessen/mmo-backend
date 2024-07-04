
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
#define BAD_STATE_PACKET 7
#define INSPECT_PLAYER_PACKET 8
#define MAP_PACKET 8

Packet *packet_template(unsigned char opcode);

// OUTGOING PACKETS
Packet *packet_template_error(int code);
Packet *packet_template_success(int code);
Packet *packet_template_failure(int code);
Packet *packet_template_BAD_STATE_PACKET(int code);
Packet *packet_template_update_position(int code, char *id, int x, int y);
Packet *packet_template_player(int code, char *name, int x, int y);

#endif