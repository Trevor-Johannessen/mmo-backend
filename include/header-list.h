#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "buffered-reader.h"

#ifndef HEADERLIST_H
#define HEADERLIST_H

typedef struct header_node {
    char *header_name;
    char *header_value;
    struct header_node *next_header;
} HeaderList;

HeaderList *header_list_parse_headers(BufferedReader *br);
void header_list_destroy(HeaderList *head);
void header_list_print(HeaderList *head);
char *header_list_get_header(HeaderList *head, char *header);
void header_add_header(HeaderList *head, char *header_name, char *header_value);

#define MAX_HEADER 128 // header names cannot exceed 127 characters
#define MAX_HEADER_VALUE 1024

#endif