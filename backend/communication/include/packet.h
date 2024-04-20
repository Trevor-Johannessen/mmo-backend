
#ifndef PACKET_H
#define PACKET_H

#include "../../networking/include/web-socket.h"

typedef struct {
	unsigned char opcode;
	unsigned long length;
	char *data;	
} Packet;

Packet *packet_read(int fd); // returns null if bad packet (invalid opcode)
void packet_write(int fd, Packet *pkt);
Packet *packet_create(unsigned char opcode, void *data, long len);
void packet_free(Packet *packet);
void *packet_flatten(Packet *packet);

#endif