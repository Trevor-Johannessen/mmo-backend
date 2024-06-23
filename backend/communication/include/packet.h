
#ifndef PACKET_H
#define PACKET_H

#include "../../networking/include/web-socket.h"

/*
	TO CREATE A NEW PACKET:

	1. Add its id to packet-types.h
	2. Define its template in packet-types.h
	3. Declare its template in packet-types.c
	4. If a recieved packet, define its handler to packet-handlers.h and packet-handlers.c
	5. Add its handler to packet_handle_route() in packet-handlers.c
	6. Add its macro to any session states it should be valid in in sessions.c
*/

#define PACKET_SIZE 1+4+4

typedef struct {
	unsigned char opcode;
	unsigned int id;
	unsigned int length;
	char *data;	
} Packet;

Packet *packet_read(int fd); // returns 0x0 if bad packet (invalid opcode)
void packet_write(int fd, Packet *pkt);
Packet *packet_create(unsigned char opcode, int id, void *data, int len);
void packet_free(Packet *packet);
void *packet_flatten(Packet *packet);

#endif