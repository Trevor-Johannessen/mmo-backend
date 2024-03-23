#include "include/web-socket.h"

void ws_create(int fd, HeaderList *headers){
    char *key = header_list_get_header(headers, "Sec-WebSocket-Key");
    char *handshake_msg = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
    unsigned char *hash;
    // hash the key
    hash = hash_key(key);
    
    // reencode the key
    key = base64_encode_glib(hash);
    free(hash);

    // Form the response packet
    fprintf(stdout, "Writing to websocket.\n");
    // note, may have to add an optional protocol is one is requested
    // in our case, the header would be 'Sec-WebSocket-Protocol: chat\r\n'
    ws_write(fd, handshake_msg, strlen(handshake_msg));
    ws_write(fd, key, strlen(key));
    ws_write(fd, "\r\n\r\n", strlen("\r\n\r\n"));

    fprintf(stdout, "Wrote to websocket.\n");
    ws_echo_server(fd);
}

void ws_run(int fd){
    // char *message;
    // message = br_read_line(br);
    // fprintf(stdout, "%s\n", message);
    // while(1);
}

char *hash_key(char* key){
    const char *suffix = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    char *concatenation;
    unsigned char *hash;
    char *hex_hash;
    int concat_len;

    if(!key) return 0x0;

    // Concatenate key with suffix
    concat_len = strlen(key) + strlen(suffix);
    concatenation = (char*)malloc(concat_len+1);
    concatenation[concat_len] = '\0';
    strcpy(concatenation, key);
    strcpy(concatenation+strlen(key), suffix);
    hash = (char *)malloc(SHA_DIGEST_LENGTH);

    // Hash the concatenation
    SHA1((const unsigned char *)concatenation, concat_len, hash);
    printf("Input String: %s\n", concatenation);
    printf("SHA-1 Hash: ");

    print_hash(hash);
    return hash;
}

void print_hash(unsigned char *hash){
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

void ws_write(int fd, void *string, long len){
    int total_wrote=0;
    int amount_wrote;
    while((amount_wrote = write(fd, string+total_wrote, len-total_wrote)) > 0)
        total_wrote+=amount_wrote;
}

void ws_read(int fd, void *buf, long len){
    int total_read=0;
    int amount_read;
    while((amount_read = read(fd, buf+total_read, len-total_read)) > 0)
        total_read+=amount_read;
}

char* base64_encode_glib(const char* input_string) {
    gsize encoded_length;
    gchar *encoded_data;
    
    // Encode the input string to base64
    encoded_data = g_base64_encode((const guchar*)input_string, SHA_DIGEST_LENGTH);
    
    return encoded_data;
}

WS_Frame *ws_text_frame(char *data) {
    int len;
    len = strlen(data);
    return ws_bin_frame((void *)data, len);
}

WS_Frame *ws_bin_frame(void *data, long len) {
    WS_Frame *frame;

    // decide how many bytes are needed for size
    if(len > 126)
        frame->length = len;
    else if(len > 32767){
        frame->length = 126;
        frame->length_ext.short_len = len;
    } else {
        frame->length = 127;
        frame->length_ext.long_len = len;
    }

    frame = malloc(sizeof(WS_Frame) + len);
    frame->fin=1;
    frame->rsv1=0;
    frame->rsv2=0;
    frame->rsv3=0;
    frame->opcode=2;
    frame->mask=0;
    memcpy(frame->data, data, len);
    return frame;
}

void ws_write_frame(int fd, WS_Frame *frame){
    long data_length = frame->length;
    ws_write(fd, frame, 2);
    if(frame->length == 126){
        ws_write(fd, &frame->length_ext.short_len, sizeof(short));
        data_length = frame->length_ext.short_len;
    }else if(frame->length == 127)
        ws_write(fd, &frame->length_ext.long_len, sizeof(long));
        data_length = frame->length_ext.long_len;
    if(frame->mask)
        ws_write(fd, &frame->key, sizeof(int));
    ws_write(fd, &frame->data, data_length);
}

void *ws_read_frame(int fd){
    WS_Frame temp_frame, *frame;
    long frame_size;
    unsigned short headers;
    char *body;

    headers = 0;
    ws_read(fd, &headers, 2);
    headers = ntohs(headers);
    temp_frame.fin = headers >> 7+8;
    temp_frame.rsv1 = headers >> 6+8;
    temp_frame.rsv2 = headers >> 5+8;
    temp_frame.rsv3 = headers >> 4+8;
    temp_frame.opcode = headers >> 8;
    temp_frame.mask = headers >> 7;
    temp_frame.length = headers & 7;

    if(temp_frame.length == 126){
        ws_read(fd, &temp_frame.length_ext.short_len, sizeof(short));
        frame = malloc(sizeof(WS_Frame) + temp_frame.length_ext.short_len);
        frame_size = temp_frame.length_ext.short_len;
    } else if(temp_frame.length == 127) {
        ws_read(fd, &temp_frame.length_ext.long_len, sizeof(long));
        frame = malloc(sizeof(WS_Frame) + temp_frame.length_ext.long_len);
        frame_size = temp_frame.length_ext.long_len;
    } else {
        frame = malloc(sizeof(WS_Frame) + temp_frame.length);
        frame_size = temp_frame.length;
    }
    if(temp_frame.mask)
        ws_read(fd, &temp_frame.key, sizeof(int));
    temp_frame.key = ntohl(temp_frame.key);
    memcpy(frame, &temp_frame, sizeof(WS_Frame));

    body = malloc(frame_size+1);
    body[frame_size] = '\0';
    ws_read(fd, body, frame_size);
    frame->data = body;
    ws_apply_key(frame->key, frame_size, frame->data);

    return frame;
}

void ws_apply_key(int key, long len, char *data){
    int i;
    for(i=0;i<len;i++){
        data[i] = data[i]^(((char*)&key)[3-(i%4)]);
    }
}

void ws_echo_server(int fd){
    struct pollfd poll_args;
    WS_Frame *frame;

    // set up polling
    poll_args.fd = fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;
    
    // start echo loop
    while(1){
    
        // await input
        while(poll(&poll_args, 1, -1) <= 0);

        // read in frame
        frame = ws_read_frame(fd);
        ws_write_frame(fd, frame);

        // echo frame back
        free(frame);
    }
}