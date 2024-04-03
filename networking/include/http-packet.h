#include "header-list.h"

#ifndef HTTPPACKET_H
#define HTTPPACKET_H

typedef struct {
    char *method;
    char *endpoint;
    char *protocol;
    HeaderList *headers;
} HTTPPacket;

#endif