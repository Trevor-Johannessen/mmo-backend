#include "include/packet.h"

Packet *packet_read(int fd){
    WS_Frame *frame;
    Packet *packet;
    unsigned long frame_len;
    int offset;
    
    // create new packet
    packet = malloc(sizeof(Packet));

    // recieve and parse binary frame
    frame = ws_read_frame(fd);
    packet->session_id = *((int *)((&frame->data)+sizeof(packet->opcode)));
    offset = sizeof(packet->opcode)+sizeof(packet->session_id);
    packet->length = ws_length(frame)-offset;
    packet->opcode = *((char *)(&frame->data));
    packet->data = frame->data-offset;

    // debug
    fprintf(stdout, "opcode: %d\nid: %ld\nlength: %ld\n", packet->opcode, packet->length, packet->session_id);

    // free frame but save the body
    frame->data=0x0;
    ws_free_frame(frame);
    return packet;
}

void *packet_flatten(Packet *packet){
    void *data;
    int header_len;
    header_len = sizeof(Packet)-sizeof(void *);
    data = malloc(packet->length+header_len);
    memcpy(data, packet, header_len);
    memcpy(data+header_len, packet->data, packet->length);
    return data;
}

void packet_write(int fd, Packet *packet){
    WS_Frame *frame;
    void *body;
    body = packet_flatten(packet);
    frame = ws_bin_frame(body, sizeof(Packet)-sizeof(void *)+packet->length);
    packet_free(packet);
    ws_write_frame(fd, frame);
    ws_free_frame(frame);
}

Packet *packet_create(unsigned char opcode, long id, void *data, long len){
    Packet *packet;
    packet = malloc(sizeof(Packet));
    packet->opcode = opcode;
    packet->session_id = id;
    packet->length = len;
    packet->data = data;
    return packet;
}

void packet_free(Packet *packet){
    if(packet->data)
        free(packet->data);
    free(packet);
}