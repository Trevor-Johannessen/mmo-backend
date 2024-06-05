#include "../../authentication/include/awaiting-connections.h"
#include "state.h"
#include "../../communication/include/packet-types.h"
#include "../../communication/include/packet.h"
#include "../../structures/include/linked-list.h"

#ifndef SESSION_H
#define SESSION_H

#include "player.h"

typedef struct session {
	int fd;
	State state;
	struct player *player;
} Session;

// List of variably sized arrays
extern StateArray **session_valid_packets;

Session *session_create(struct player *player, int fd);
void session_destroy(Session *session);
int session_verify_packet(Session *session, Packet *packet);
void session_populate_list();

#endif