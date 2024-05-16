#ifndef STATE_H
#define STATE_H

typedef enum {
	DISABLED, //(Allow no packets except login)
	ROAMING, //(Allow movement and interaction related packets)
	//etc... (This enum is expandable)
	FOOTER // This is to easily get the number of states
} State;

typedef struct {
	int size;
	State valid_packet_types[];
} StateArray;

#endif