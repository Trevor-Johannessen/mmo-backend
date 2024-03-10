#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // sleep for linux

void *start_auth_reciever(void* port);
void *web_server_start(void* port);

char *AUTH_PORT = "12000";
char *WEB_PORT = "12001";
const int SPIN_SEPERATE_AUTH = 0;

int main(int argc, char* argv[]){
    // Spin up auth-reciever thread
    pthread_t auth_reciever_tid;
    pthread_t web_server_tid;
    if(pthread_create(&auth_reciever_tid, NULL, start_auth_reciever, AUTH_PORT)){
        fprintf(stderr, "Could not start auth reciever thread.\n");
        return 0;
    }
    if(pthread_create(&web_server_tid, NULL, web_server_start, WEB_PORT)){
        fprintf(stderr, "Could not start auth reciever thread.\n");
        return 0;
    }
    while(1){
        sleep(60);
    }
    return 1;
}