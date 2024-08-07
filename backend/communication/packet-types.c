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

Packet *packet_template_update_position(char *id, int x, int y){
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

    return packet_create(MOVED_PACKET, 0, body, size);
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

Packet *packet_template_map(int map_id){
    Map *map;
    Packet *packet;
    void *body;
    int packet_size;
    
    // ref new map by id
    map = map_load(map_id);

    // init packet
    packet_size = sizeof(int)*2 + map->width*map->height;
    body = malloc(packet_size);

    // add dimensions
    ((int *)(body))[0] = map->width;
    ((int *)(body))[1] = map->height;

    // add decor string (don't need collision string since collision is handled by the server)
    strncpy(body+sizeof(int)*2, map->decor, map->width*map->height);

    return packet_create(MAP_PACKET, 0, body, packet_size);
}

Packet *packet_template_leaving_map(char *id){
    void *body;
    int packet_size;

    packet_size = strlen(id);
    body = malloc(packet_size+1);
    strncpy(body, id, packet_size+1);

    return packet_create(LEAVING_MAP_PACKET, 0, body, packet_size);
}