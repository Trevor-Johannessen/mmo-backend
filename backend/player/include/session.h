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
	State state;
	char *id; // (currently static, may need to use inital code as seed for incrementing code)
	int fd;
} Session;

// List of variably sized arrays
extern StateArray **session_valid_packets;

Session *session_create(Player *player, char *id, int fd);
void session_destroy(Session *session);
int session_verify_packet(Session *session, unsigned char opcode);
void session_populate_list();

#endif