#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "../../communication/include/packet.h"
#include "../../communication/include/packet-types.h"
#include "../../player/include/session.h"

/*
    This is the loop that is responsible for reading using input, and directing it to the correct functioins

    It should follow this general format:

    1. Read packet
    2. Plan route for packet
    3. Deliver packet to routes
    4. Return to function
    5. Cleanup
    6. Repeat
*/

void event_loop_start(int fd);

#endif