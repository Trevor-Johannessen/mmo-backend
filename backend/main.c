#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // sleep for linux
#include <time.h>
#include "structures/include/linked-list.h"
#include "player/include/session.h"
#include "maps/include/map.h"

void *start_auth_reciever(void* port);
void *web_server_start(void* port);

char *AUTH_PORT = "12000";
char *WEB_PORT = "12001";
const int SPIN_SEPERATE_AUTH = 0;

int main(int argc, char* argv[]){
    // preparation
    srand(time(0x0));
    tile_lock_init();
    session_populate_list();

    // Spin up auth-reciever thread
    pthread_t auth_reciever_tid;
    pthread_t web_server_tid;
    fprintf(stdout, "Starting auth reciever...\n");
    if(pthread_create(&auth_reciever_tid, 0x0, start_auth_reciever, AUTH_PORT)){
        fprintf(stderr, "Could not start auth reciever thread.\n");
        return 0;
    }
    // if(pthread_create(&web_server_tid, 0x0, web_server_start, WEB_PORT)){
    //     fprintf(stderr, "Could not start auth reciever thread.\n");
    //     return 0;
    // }
    
    fprintf(stdout, "Starting web server...\n");
    web_server_start(WEB_PORT);
    return 1;
}