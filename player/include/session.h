#include "../../authentication/include/awaiting-connections.h"
#include "player.h"
#include "state.h"
#include "../../communication/include/packet-types.h"
#include "../../communication/include/packet.h"
#include "../../structures/include/linked-list.h"

#ifndef SESSION_H
#define SESSION_H

typedef struct {
	Player *player;
	long id; // (currently static, may need to use inital code as seed for incrementing code)
	State state;
} Session;

// List of variably sized arrays
extern StateArray **session_valid_packets;

Session *session_create(Player *player, long id);
void session_destroy(Session *session);
int session_verify_packet(Session *session, unsigned char opcode);
void session_populate_list();

#endif