#include "session.h"
#include "../../networking/include/web-socket.h"

#ifndef PACKET_H
#define PACKET_H

typedef struct {
	unsigned char opcode;
	long session_id;
	char *data;	
} Packet;

Packet *packet_read(int fd, Session *session); // returns null if bad packet (invalid opcode or bad session_id)
void packet_write(int fd, Packet *pkt);
Packet *packet_create(unsigned char opcode, char *data);

#endif