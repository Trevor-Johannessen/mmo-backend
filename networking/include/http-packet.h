#include "header-list.h"
#include "buffered-reader.h"

typedef struct {
    char *method;
    char *endpoint;
    char *protocol;
    HeaderList *headers;
} HTTPPacket;


HTTPPacket *http_packet_get(BufferedReader br);
int http_packet_send(int fd, HTTPPacket *packet);