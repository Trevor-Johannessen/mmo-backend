
#ifndef PACKETTYPES_H
#define PACKETTYPES_H

#include "packet.h"
#include "errors.h"
#include "../../maps/include/map.h"

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
#define MAP_PACKET 9
#define LEAVING_MAP_PACKET 10

Packet *packet_template(unsigned char opcode);

// OUTGOING PACKETS
Packet *packet_template_error(int code);
Packet *packet_template_success(int code);
Packet *packet_template_failure(int code);
Packet *packet_template_bad_state(int code);
Packet *packet_template_update_position(char *id, int x, int y);
Packet *packet_template_player(int code, char *name, int x, int y);
Packet *packet_template_map(int map_id);
Packet *packet_template_leaving_map(char *id);

#endif