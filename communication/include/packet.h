#include "session.h"
#include "../../networking/include/web-socket.h"

#ifndef PACKET_H
#define PACKET_H

// Client Opcodes
// #define C_NAME code

// Server Opcodes
// #define S_NAME code

typedef struct {
	unsigned char opcode;
	unsigned long session_id;
	unsigned long length;
	char *data;	
} Packet;

Packet *packet_read(int fd, Session *session); // returns null if bad packet (invalid opcode or bad session_id)
void packet_write(int fd, Packet *pkt);
Packet *packet_create(unsigned char opcode, long id, void *data, long len);
void packet_free(Packet *packet);
void *packet_flatten(Packet *packet);

#endif