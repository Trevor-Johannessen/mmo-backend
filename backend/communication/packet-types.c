#include "include/packet-types.h"

Packet *packet_template(unsigned char opcode){
    
}

// OUTGOING PACKETS

Packet *packet_template_success(int code){
    return packet_create(SUCCESS_PACKET, code, 0, 0);
}

Packet *packet_template_failure(int code){
    return packet_create(FAILURE_PACKET, code, 0, 0);
}

Packet *packet_template_BAD_STATE_PACKET(int code){
    return packet_create(BAD_STATE_PACKET, code, 0, 0);
}

Packet *packet_template_error(int code){
    char *msg;
    if(code > (error_length-1))
        code = 0;
    msg = malloc(strlen(error_strings[code]));
    strcpy(msg, error_strings[code]);
    return packet_create(ERROR, code, msg, strlen(msg));
}

Packet *packet_template_update_position(int code, char *id, int x, int y){
    int size, length;
    char *body;

    if(!id)
        return 0x0;

    length = strlen(id);
    size = sizeof(int)*2 + length;
    body = malloc(size);

    ((int *)body)[0] = x;
    ((int *)body)[1] = y;
    strncpy(body+sizeof(int)*2, id, length);

    return packet_create(MOVED_PACKET, code, body, size);
}

Packet *packet_template_player(int code, char *name, int x, int y){
    char *body;
    int size, static_size;

    static_size = sizeof(x) + sizeof(y);
    size = static_size + strlen(name)+1;
    body = malloc(size);
    ((int *)(body))[0] = x;
    ((int *)(body))[1] = y;
    strncpy(body+static_size, name, strlen(name)+1);

    return packet_create(INSPECT_PLAYER_PACKET, code, body, size);
}