#ifndef FUNNEL_H
#define FUNNEL_H

#define _GNU_SOURCE // needed for some linux thread scheduling

#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include "linked-list.h"
#include "../../communication/include/packet.h"

#define MAX_FUNNEL_THREADS 10

typedef struct {
    int new_thread_interval; // size queue needs to grow before the next thread is created
    int active; // boolean for whether the funnel is in sleep mode or not
    int enabled; // boolean for manually enabling/disabling the loop
    atomic_int thread_count;
    atomic_int queue_size;
    pthread_t tids[MAX_FUNNEL_THREADS]; 
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;
    pthread_mutex_t thread_lock;
    Link *head;
    Link *tail;
    void (*func)(void *); // operator on queue payloads
    void (*free_payload_func)(void *); // free function for queue payloads
} Funnel;

Funnel *funnel_create();
void funnel_free(Funnel *funnel, int drop_payloads);
void funnel_enqueue(Funnel *funnel, void *link);
Link *funnel_dequeue(Funnel *funnel);
void funnel_expand(Funnel *funnel);
void funnel_contract(Funnel *funnel);
void funnel_sleep(Funnel *funnel, int index);
void funnel_wakeup(Funnel *funnel, int index);
void *funnel_thread_start(void *funnel);

#endif