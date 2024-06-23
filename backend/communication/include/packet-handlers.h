#ifndef PACKETHANDLERS_H
#define PACKETHANDLERS_H

#include "packet-types.h"
#include "packet.h"
#include "errors.h"
#include "../../player/include/session.h"
#include "../../db/include/db.h"
#include "../../authentication/include/awaiting-connections.h"
#include "../../db/include/db-player.h"

Packet *packet_handle_route(Packet *packet, Session *session);
Packet *packet_handle_login(Packet *login_packet, Session *session);
Packet *packet_handle_logout(Packet *logout_packet, Session *session);
Packet *packet_handle_success(Packet *success_packet, Session *session);
Packet *packet_handle_failure(Packet *failure_packet, Session *session);
Packet *packet_handle_move(Packet *move_packet, Session *session);

#endif