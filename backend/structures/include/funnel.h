#ifndef FUNNEL_H
#define FUNNEL_H

#include <pthread.h>
#include <sched.h>
#include <stdatomic.h>
#include "linked-list.h"
#include "../../communication/include/packet.h"

#define MAX_FUNNEL_THREADS 3

typedef struct {
    int new_thread_interval;
    int thread_count;
    int active;
    pthread_mutex_t head_lock;
    pthread_mutex_t tail_lock;
    pthread_mutex_t thread_lock;
    Link *head;
    Link *tail;
    void (*func)(void *);
    void (*free_payload_func)(void *);
    atomic_int queue_size;
    pthread_t tids[MAX_FUNNEL_THREADS]; // threshold for reaping thread should be smaller than threshold for creating new threads (to not clobber)
} Funnel;

Funnel *funnel_create();
void funnel_free(Funnel *funnel, int drop_packets);
void funnel_enqueue(Funnel *funnel, Link *link);
Link *funnel_dequeue(Funnel *funnel);
void funnel_sleep(Funnel *funnel);
void funnel_wakeup(Funnel *funnel);
void *funnel_thread_start(void *funnel);

#endif