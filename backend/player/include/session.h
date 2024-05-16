#include "../../authentication/include/awaiting-connections.h"
#include "player.h"
#include "state.h"
#include "../../communication/include/packet-types.h"
#include "../../communication/include/packet.h"
#include "../../structures/include/linked-list.h"

#ifndef SESSION_H
#define SESSION_H

typedef struct {
	int fd;
	State state;
	Player *player;
} Session;

// List of variably sized arrays
extern StateArray **session_valid_packets;

Session *session_create(Player *player, int fd);
void session_destroy(Session *session);
int session_verify_packet(Session *session, Packet *packet);
void session_populate_list();

#endif