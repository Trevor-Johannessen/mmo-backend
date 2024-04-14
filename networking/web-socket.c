#include "include/web-socket.h"

const long MB = 1048576;
const long WS_FRAME_SIZE = 4 * MB;

void ws_create(int fd, HeaderList *headers){
    char *key = header_list_get_header(headers, "Sec-WebSocket-Key");
    char *handshake_msg = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";
    unsigned char *hash;
    // hash the key
    hash = hash_key(key);
    
    // reencode the key
    key = g_base64_encode((const guchar*)hash, SHA_DIGEST_LENGTH);
    //key = base64_encode_glib(hash);
    free(hash);

    // Form the response packet
    fprintf(stdout, "Writing to websocket.\n");
    // note, may have to add an optional protocol is one is requested
    // in our case, the header would be 'Sec-WebSocket-Protocol: chat\r\n'
    ws_write(fd, handshake_msg, strlen(handshake_msg));
    ws_write(fd, key, strlen(key));
    ws_write(fd, "\r\n\r\n", strlen("\r\n\r\n"));
    free(key);
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
    free(concatenation);
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

unsigned short ws_transcribe_headers(WS_Frame *frame){
    short headers;
    headers = 0;
    headers |= frame->fin<<7;
    headers |= frame->rsv1<<6;
    headers |= frame->rsv2<<5;
    headers |= frame->rsv3<<4;
    headers |= frame->opcode;
    headers |= frame->mask<<7+8;
    headers |= frame->length<<8;
    return headers;
}

unsigned long ws_length(WS_Frame *frame){
    if(frame->length == 126){
        return (long)(frame->length_ext.short_len);
    } else if(frame->length == 127) {
        return frame->length_ext.long_len;
    } else {
        return (long)frame->length;
    }
}

WS_Frame *ws_text_frame(char *data) {
    WS_Frame *frame;
    int len;
    len=-1;
    if(data)
        len = strlen(data);
    frame =  ws_bin_frame((void *)data, len+1);
    frame->opcode = 1;
    return frame;
}

WS_Frame *ws_close_frame(short *status_code){
    WS_Frame *frame;
    frame =  ws_bin_frame((void *)status_code, sizeof(*status_code));
    frame->opcode = 8;
    return frame;
}

WS_Frame *ws_bin_frame(void *data, long len) {
    WS_Frame *frame;
    frame = malloc(sizeof(WS_Frame));
    // decide how many bytes are needed for size
    if(len < 126)
        frame->length = len;
    else if(len > 32767){
        frame->length = 126;
        frame->length_ext.short_len = len;
    } else {
        frame->length = 127;
        frame->length_ext.long_len = len;
    }

    frame->fin=1;
    frame->rsv1=0;
    frame->rsv2=0;
    frame->rsv3=0;
    frame->opcode=2;
    frame->mask=0;
    frame->data = malloc(len);
    memcpy(frame->data, data, len);
    return frame;
}

void ws_free_frame(WS_Frame *frame){
    if(frame->data)
        free(frame->data);
    free(frame);
}

void ws_write_frame(int fd, WS_Frame *frame){
    long data_length;
    unsigned short headers;
    data_length = frame->length;
    headers = ws_transcribe_headers(frame);
    ws_write(fd, &headers, sizeof(short));
    if(frame->length == 126){
        ws_write(fd, &frame->length_ext.short_len, sizeof(short));
        data_length = frame->length_ext.short_len;
    }else if(frame->length == 127){
        ws_write(fd, &frame->length_ext.long_len, sizeof(long));
        data_length = frame->length_ext.long_len;
    }
    if(frame->mask)
        ws_write(fd, &frame->key, sizeof(int));
    ws_write(fd, frame->data, data_length);
}

WS_Frame *ws_read_frame(int fd){
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
    temp_frame.length = headers & 127;

    // get frame size
    if(temp_frame.length == 126){
        ws_read(fd, &temp_frame.length_ext.short_len, sizeof(short));
        frame = malloc(sizeof(WS_Frame) + temp_frame.length_ext.short_len);
        frame_size = temp_frame.length_ext.short_len;
    } else if(temp_frame.length == 127) {
        ws_read(fd, &temp_frame.length_ext.long_len, sizeof(long));
        frame_size = temp_frame.length_ext.long_len;
        // Check for invalid sizes
        if(frame_size > WS_FRAME_SIZE)
            return NULL;   
        frame = malloc(sizeof(WS_Frame) + temp_frame.length_ext.long_len);
    } else {
        frame_size = temp_frame.length;
        frame = malloc(sizeof(WS_Frame) + temp_frame.length);
    }
    
    // get mask
    if(temp_frame.mask)
        ws_read(fd, &temp_frame.key, sizeof(int));
    temp_frame.key = ntohl(temp_frame.key);

    // transfer frame
    memcpy(frame, &temp_frame, sizeof(WS_Frame));

    // read body
    if(frame_size){
        body = malloc(frame_size+1);
        body[frame_size] = '\0';
        ws_read(fd, body, frame_size);
        frame->data = body;
        ws_apply_key(frame->key, frame_size, frame->data);
    }else{
        frame->data = 0x0;
    }

    // Check for invalid frames
    if((frame->opcode > 2 && frame->opcode < 8) || (frame->opcode > 10)){
        ws_free_frame(frame);
        return NULL;
    }

    return frame;
}

void ws_apply_key(int key, long len, char *data){
    int i;
    for(i=0;i<len;i++){
        data[i] = data[i]^(((char*)&key)[3-(i%4)]);
    }
}

void ws_close(int fd, int code){
    WS_Frame *frame;
    unsigned short *code_buf;
    code_buf = malloc(sizeof(code_buf));
    *code_buf = htons(code);
    frame = ws_close_frame(code_buf);
    ws_write_frame(fd, frame);
    ws_free_frame(frame);
    free(code_buf);
}

void ws_echo_server(int fd){
    struct pollfd poll_args;
    WS_Frame *r_frame, *w_frame;
    r_frame = w_frame = NULL;
    
    // set up polling
    poll_args.fd = fd;
    poll_args.events = POLLIN;
    poll_args.revents = 0;
    
    // start echo loop
    //while(1){
    for(int i=0; i<3; i++){
    
        // await input
        while(poll(&poll_args, 1, -1) <= 0);
        // read in frame
        r_frame = ws_read_frame(fd);

        if(!r_frame || r_frame->opcode == WS_CLOSE){
            break;
        }

        // create return frame
        w_frame = ws_text_frame(r_frame->data);
        ws_write_frame(fd, w_frame);

        // echo frame back
        ws_free_frame(r_frame);
        ws_free_frame(w_frame);
    }
    ws_close(fd, 1000);
    fprintf(stdout, "Closing connection.\n");
}