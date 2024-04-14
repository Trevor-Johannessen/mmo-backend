#ifndef HTTPPACKET_H
#define HTTPPACKET_H

#include "header-list.h"

typedef struct {
    char *method;
    char *endpoint;
    char *protocol;
    HeaderList *headers;
} HTTPPacket;

#endif