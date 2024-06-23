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

Packet *packet_template_bad_state(int code){
    return packet_create(BAD_STATE, code, 0, 0);
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
    int size;
    char *body;

    if(!id)
        return 0x0;

    size = sizeof(int)*2 + strlen(id);
    body = malloc(size);

    ((int *)body)[0] = x;
    ((int *)body)[1] = y;
    strcpy(body+sizeof(int)*2, id);

    return packet_create(MOVED_PACKET, code, body, size);
}