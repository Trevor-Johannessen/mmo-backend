#ifndef STATE_H
#define STATE_H

typedef enum {
	DISABLED, //(Allow no packets except login)
	MOVING, //(Allow movement and interaction related packets)
	IN_MENU, //(Allow menu-related packets)
	PAUSED, //(Allow unpause packet)
	IN_GAME, //(For minigame specific packets, 1 enum per game)
	IN_CUSTOM_SCREEN, //(For misc screens that need special packets)
	//etc... (This enum is expandable)
	FOOTER // This is to easily get the number of states
} State;

typedef struct {
	int size;
	State valid_packet_types[];
} StateArray;

#endif