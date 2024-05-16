#include "include/errors.h"

char *error_strings[] = {
    "UNDEFINED ERROR",
    "INVALID LOGIN PACKET"
};

int error_length = sizeof(error_strings) / sizeof(error_strings[0]);
int packet_errno = 0;