#include "../../authentication/include/awaiting-connections.h"
#include "player.h"

#ifndef SESSION_H
#define SESSION_H

typedef enum {
	MOVING, //(Allow movement and interaction related packets)
	IN_MENU, //(Allow menu-related packets)
	PAUSED, //(Allow unpause packet)
	IN_GAME, //(For minigame specific packets, 1 enum per game)
	IN_CUSTOM_SCREEN, //(For misc screens that need special packets)
	DISABLED, //(Allow no packets except logout)
	//etc... (This enum is expandable)
} State;


typedef struct {
	Player *player;
	long session_id; // (currently static, may need to use inital code as seed for incrementing code)
	State state;
} Session;

int session_create(Player *player, long id);
void session_destroy(Session *session);
void sesion_set_state(State *state);

#endif