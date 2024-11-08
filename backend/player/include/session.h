#include "../../authentication/include/awaiting-connections.h"
#include "state.h"
#include "../../communication/include/packet-types.h"
#include "../../communication/include/packet.h"
#include "../../structures/include/linked-list.h"

#ifndef SESSION_H
#define SESSION_H

typedef struct session {
	int fd;
	struct player *player;
} Session;

Session *session_create(int fd);
void session_destroy(Session *session);

#endif