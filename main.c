#include <pthread.h>
#include <stdio.h>

void *start_auth_reciever(void* port);

char* AUTH_PORT = "12000";
const int SPIN_SEPERATE_AUTH = 0;

int main(int argc, char* argv[]){
    // Spin up auth-reciever thread
    if(SPIN_SEPERATE_AUTH){
        pthread_t auth_reciever_tid;
        if(pthread_create(&auth_reciever_tid, NULL, start_auth_reciever, AUTH_PORT)){
            fprintf(stderr, "Could not start auth reciever thread.\n");
            return 0;
        }
        pthread_join(auth_reciever_tid, NULL);
    }else{
        start_auth_reciever(AUTH_PORT);
    }
    return 1;
}