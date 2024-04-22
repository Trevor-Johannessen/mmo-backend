#include "include/session.h"

StateArray **session_valid_packets;

Session *session_create(Player *player, char *id, int fd){
    Session *session;
    
    session = malloc(sizeof(session));
    memset(session, 0, sizeof(session));
    session->player = player;
    session->id = id;
    session->state = DISABLED;
    session->fd = fd;
    return session;
}

void session_destroy(Session *session){
    if(!session)
        return;
    player_free(session->player);
    free(session->id);
    free(session);
}

int session_verify_packet(Session *session, unsigned char opcode){
    // get current state array
    // binary search to find if opcode is in array
    return 1;
}

void session_populate_list(){
    State length;
    int state_count;

    length = FOOTER;
    session_valid_packets = malloc(((int)length-1) * sizeof(StateArray *));

    /* !!! ARRAY MUST BE SORTED !!! */
    /* This syntax is bad, please fix if possible */


    // DISABLED
    session_valid_packets[0] = malloc(sizeof(int) + sizeof(State)*1);
    session_valid_packets[0]->size = 1;
    StateArray disabled = {LOGIN_PACKET};
    memcpy(session_valid_packets[0]->valid_packet_types, &disabled, sizeof(disabled));

	// MOVING
    session_valid_packets[1] = malloc(sizeof(int) + sizeof(State)*1);
    session_valid_packets[1]->size = 1;
    StateArray moving = {LOGIN_PACKET};
    memcpy(session_valid_packets[0]->valid_packet_types, &moving, sizeof(moving));

	// IN_MENU
    session_valid_packets[2] = malloc(sizeof(int) + sizeof(State)*1);
    session_valid_packets[2]->size = 1;
    StateArray in_menu = {LOGIN_PACKET};
    memcpy(session_valid_packets[0]->valid_packet_types, &in_menu, sizeof(in_menu));

	// PAUSED
    session_valid_packets[3] = malloc(sizeof(int) + sizeof(State)*1);
    session_valid_packets[3]->size = 1;
    StateArray paused = {LOGIN_PACKET};
    memcpy(session_valid_packets[0]->valid_packet_types, &paused, sizeof(paused));
    
    // IN_GAME
    session_valid_packets[4] = malloc(sizeof(int) + sizeof(State)*1);
    session_valid_packets[4]->size = 1;
    StateArray in_game = {LOGIN_PACKET};
    memcpy(session_valid_packets[0]->valid_packet_types, &in_game, sizeof(in_game));

	// IN_CUSTOM_SCREEN
    session_valid_packets[5] = malloc(sizeof(int) + sizeof(State)*1);
    session_valid_packets[5]->size = 1;
    StateArray in_custom_screen = {LOGIN_PACKET};
    memcpy(session_valid_packets[0]->valid_packet_types, &in_custom_screen, sizeof(in_custom_screen));
	
    // FOOTER
    session_valid_packets[6] = malloc(sizeof(int) + sizeof(State)*0);
    session_valid_packets[6]->size = 0;

}

void session_destroy_list(){
    State length;
    int i;

    length = FOOTER;
    for(i=0; i<length; i++){
        free(session_valid_packets[i]);
    }
}