#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "include/buffered_reader.h"

BufferedReader *br_init(int buf_size, int fd){
    BufferedReader *buffer;

    if(!(buffer = (BufferedReader *)malloc(sizeof(BufferedReader)))){
        return 0x0;
    }
    if(!(buffer->buf = (char *)malloc(sizeof(char) * buf_size))){
        free(buffer);
        return 0x0;
    }
    memset(buffer->buf, '\0', buf_size);
    buffer->buf_size = buf_size;
    buffer->fd = fd;
    return buffer;
}

void br_destroy(BufferedReader *br){
    if(!br)
        return;
    close(br->fd);
    free(br->buf);
    free(br);
}

char* br_read_line(BufferedReader *br, int *flag){
    int i;
    char *out;

    *flag = 0;
    out = 0x0;
    while(1){
        for(i=0; i<br->buf_size-1 && br->buf[i] != '\n'; i++); // assuming that \r always follows \n
        if(i==0){// No data in buffer
            // read new content
            br_read(br, 0);
            // repeat
            continue;
        }else if(i==br->buf_size-1 && br->buf[i] != '\n'){ // No newline in buffer
            *flag = 1; // raise flag signifying that the line was too long.
            break;
        }else{ // found a complete line
            // copy string to new buffer
            if(!(out = malloc(i+1)))
                return 0x0;
            strncpy(out, br->buf, i+1);
            // move rest of string to beginning of line
            memmove(br->buf, br->buf+i, br->buf_size-i);
            // read new content
            br_read(br, br->buf_size-i);
            return out;
        }
    }
}

int br_read(BufferedReader *br, int start){
    int total_read=0;
    int amount_read;
    while((amount_read = read(br->fd, br->buf+start+total_read, br->buf_size-start-total_read)) > 0)
        total_read+=amount_read;
    return total_read;
}
