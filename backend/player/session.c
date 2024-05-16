#include "include/session.h"

StateArray **session_valid_packets;

Session *session_create(Player *player, int fd){
    Session *session;
    
    session = malloc(sizeof(session));
    memset(session, 0, sizeof(session));
    session->player = player;
    session->state = DISABLED;
    session->fd = fd;
    return session;
}

void session_destroy(Session *session){
    if(!session)
        return;
    if(session->player)
        player_free(session->player);
    free(session);
}

int session_verify_packet(Session *session, Packet *packet){
    // get current state array
    // binary search to find if opcode is in array
    // OPTIMIZATION: Replace this O(n) search with Binary Search
    int i, opcode;
    StateArray *arr;

    opcode = packet->opcode;
    arr = session_valid_packets[session->state];
    for(i=0;i<arr->size;i++)
        if(arr->valid_packet_types[i] == opcode)
            return 1;
    return 0;
}

void session_populate_list(){
    State length;
    int state_count;

    length = FOOTER;
    session_valid_packets = malloc(((int)length-1) * sizeof(StateArray *));

    /* !!! SUB ARRAYS MUST BE SORTED (by enum value) TO BINARY SEARCH FOR CORRECT PACKET !!! */
    /* This syntax is bad, please fix if possible */


    // DISABLED
    StateArray disabled = {LOGIN_PACKET};
    session_valid_packets[0] = malloc(sizeof(int) + sizeof(State)*(sizeof(disabled) / sizeof(StateArray)));
    session_valid_packets[0]->size = sizeof(disabled) / sizeof(StateArray);
    memcpy(session_valid_packets[0]->valid_packet_types, &disabled, sizeof(disabled));

	// ROAMING
    StateArray roaming = {MOVE_PACKET};
    session_valid_packets[1] = malloc(sizeof(int) + sizeof(State)*(sizeof(roaming) / sizeof(StateArray)));
    session_valid_packets[1]->size = sizeof(roaming) / sizeof(StateArray);
    memcpy(session_valid_packets[1]->valid_packet_types, &roaming, sizeof(roaming));
	
    // FOOTER
    session_valid_packets[length] = malloc(sizeof(int) + sizeof(State)*0);
    session_valid_packets[length]->size = 0;

}

void session_destroy_list(){
    State length;
    int i;

    length = FOOTER;
    for(i=0; i<length; i++){
        free(session_valid_packets[i]);
    }
}