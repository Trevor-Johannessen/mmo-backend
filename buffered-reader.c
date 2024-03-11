#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "include/buffered-reader.h"
#include <fcntl.h>

BufferedReader *br_init(int fd, int buf_size){
    BufferedReader *buffer;
    int current_flags;

    current_flags = fcntl(fd, F_GETFL, 0x0);
    fcntl(fd, F_SETFL, current_flags | O_NONBLOCK);

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
    buffer->flags=0;
    return buffer;
}

void br_destroy(BufferedReader *br){
    if(!br)
        return;
    close(br->fd);
    free(br->buf);
    free(br);
}

char* br_read_line(BufferedReader *br){
    int i;
    char *out;

    out = 0x0;
    while(1){
        for(i=0; br->buf[i] != '\0' && i<(br->buf_size) && br->buf[i] != '\n'; i++); // assuming that \r always follows \n
        if(i==0){// No data in buffer
            // read new content
            br_read(br, 0);
            // repeat
            continue;
        }else if(i==br->buf_size && br->buf[i] != '\n'){ // No newline in buffer
            br->flags = 1; // raise flag signifying that the line was too long.
            break;
        }else{ // found a complete line
            // copy string to new buffer
            if(!(out = malloc(i+2)))
                return 0x0;
            memset(out, '\0', i+2);
            strncpy(out, br->buf, i+1);
            // move rest of string to beginning of line
            memmove(br->buf, br->buf+i+1, br->buf_size-i-2); // this shouldn't be too slow, but can attempt a circular buffer if needed.
            memset(br->buf+br->buf_size-i, '\0', i);
            // read new content
            br_read(br, br->buf_size-i-2);
            return out;
        }
    }
}

int br_read(BufferedReader *br, int start){
    int total_read=0;
    int amount_read;
    while((amount_read = read(br->fd, br->buf+start+total_read, br->buf_size-start-total_read-1)) > 0)
        total_read+=amount_read;
    return total_read;
}
