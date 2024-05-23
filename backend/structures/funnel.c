#include "include/funnel.h"

Funnel *funnel_create(){
    Funnel *funnel;
    int i;

    if(!(funnel = malloc(sizeof(Funnel))))
        return 0x0;

    // initalize
    pthread_mutex_init(&funnel->head_lock, 0x0);
    pthread_mutex_init(&funnel->tail_lock, 0x0);
    pthread_mutex_init(&funnel->thread_lock, 0x0);
    funnel->head = 0x0;
    funnel->tail = 0x0;
    funnel->thread_count = 1;
    funnel->active = 1;
    funnel->queue_size = ATOMIC_VAR_INIT(0);
    for(i=0;i<MAX_FUNNEL_THREADS;i++)
        funnel->tids[i] = 0x0;
    funnel->tids[0] = pthread_self();

    return funnel;
}

void funnel_free(Funnel *funnel, int drop_payloads){
    if(!funnel)
        return;
    
    // get exclusive access
    pthread_mutex_lock(&funnel->head_lock);
    pthread_mutex_lock(&funnel->tail_lock);
    pthread_mutex_lock(&funnel->thread_lock);

    // drop payloads if requested
    if(drop_payloads){
        link_map(funnel->head, funnel->free_payload_func);
        funnel->head = 0x0;
        funnel->tail = 0x0;
    }

    // do not free struct if packets are still in queue
    if(funnel->head){
        pthread_mutex_unlock(&funnel->head_lock);
        pthread_mutex_unlock(&funnel->tail_lock);
        pthread_mutex_unlock(&funnel->thread_lock);
        return;
    }
    
    // free struct
    pthread_mutex_destroy(&funnel->head_lock);
    pthread_mutex_destroy(&funnel->tail_lock);
    pthread_mutex_destroy(&funnel->thread_lock);
    // does atomic_int need to be freed?
    free(funnel);

}

void funnel_enqueue(Funnel *funnel, Link *link){
    int grabbed_head_mutex;

    // grab needed locks
    grabbed_head_mutex = 0;
    if(funnel->head == funnel->tail){
        pthread_mutex_lock(&funnel->head_lock);
        grabbed_head_mutex = 1;
    }
    pthread_mutex_lock(&funnel->tail_lock);

    // if no nodes in funnel, set head and tail
    if(!funnel->head){
        funnel->head = link;
        funnel->tail = link;

    // if nodes in funnel, append to tail
    } else {
        funnel->tail->next = link;
        funnel->tail = link;
    }
    atomic_fetch_add(&(funnel->queue_size), 1);

    // release locks
    if(grabbed_head_mutex)
        pthread_mutex_unlock(&funnel->head_lock);
    pthread_mutex_unlock(&funnel->tail_lock);
}

Link *funnel_dequeue(Funnel *funnel){
    int grabbed_tail_mutex;
    Link *out;

    // check if list is empty
    if(!(atomic_load(&(funnel->queue_size))))
        return 0x0;

    // grab needed locks
    pthread_mutex_lock(&funnel->head_lock);
    if(funnel->head == funnel->tail){
        pthread_mutex_lock(&funnel->tail_lock);
        grabbed_tail_mutex = 1;
    }

    // remove head if there is one node
    if(funnel->head == funnel->tail){
        out = funnel->head;
        funnel->head = 0x0;
        funnel->tail = 0x0;

    // remove head if there are multiple
    } else {
        out = funnel->head;
        funnel->head = funnel->head->next;
    }

    // release locks
    pthread_mutex_unlock(&funnel->head_lock);
    if(grabbed_tail_mutex)
        pthread_mutex_unlock(&funnel->tail_lock);

    // return head
    return out;
}

void funnel_sleep(Funnel *funnel){
    // grab lock
    pthread_mutex_lock(&funnel->thread_lock);

    // reschedule threads
    // for(i=0;i<funnel->thread_count;i++)
    //     pthread_setschedparam(funnel->tids[i], SCHED_IDLE);

    // set funnel as inactive
    funnel->active = 0;

    // release lock
    pthread_mutex_lock(&funnel->thread_lock);
}

void funnel_wakeup(Funnel *funnel){
    int i;

    // grab lock
    pthread_mutex_lock(&funnel->thread_lock);

    // reschedule threads
    // for(i=0;i<funnel->thread_count;i++)
    //     pthread_setschedparam(funnel->tids[i], SCHED_OTHER);

    // set funnel as active
    funnel->active = 1;

    // release lock
    pthread_mutex_lock(&funnel->thread_lock);
}

void funnel_thread_resize(Funnel *funnel){
    // grab lock
    pthread_mutex_lock(&funnel->thread_lock);

    // if size is zero, sleep
    if(!(atomic_load(&(funnel->queue_size)))){
        funnel_sleep(funnel);
        goto funnel_cleanup; // early leave
    }

    // if size greater than next threshold, increase threads
    while((atomic_load(&(funnel->queue_size))) > funnel->new_thread_interval*funnel->thread_count){
        funnel->thread_count++;
        pthread_create(&(funnel->tids[funnel->thread_count-1]), 0x0, funnel_thread_start, funnel);
    }

    // if size is under threshold by a large margin, decrease threads 
    while((atomic_load(&(funnel->queue_size))) < funnel->new_thread_interval*(funnel->thread_count-1)){
        // kill newest thread
        funnel->tids[funnel->thread_count-1] = 0x0;
        funnel->thread_count--;
    }
    
    // release lock
    funnel_cleanup:
    pthread_mutex_unlock(&funnel->thread_lock);
}

void *funnel_thread_start(void *funnel_in){
    Funnel *funnel;
    int tid_index, i;
    pthread_t my_tid;
    Link *link;

    funnel = (Funnel *)funnel_in;

    // find tid index in array
    my_tid = pthread_self();
    tid_index = -1;
    for(i=0;i<MAX_FUNNEL_THREADS;i++)
        if(funnel->tids[i] == my_tid)
            tid_index = i;
    if(tid_index < 0)
        return 0x0;

    while(funnel->tids[tid_index] == my_tid){
        if(!funnel->active)
            continue;

        // check if size is greater than zero
        if(!(atomic_load(&(funnel->queue_size)))){
            pthread_mutex_lock(&funnel->head_lock);
            pthread_mutex_lock(&funnel->tail_lock);
            if(!(atomic_load(&(funnel->queue_size))))
                funnel_thread_resize(funnel);
            // unlocking is low priority, will this cause delayed packets???
            pthread_mutex_unlock(&funnel->head_lock);
            pthread_mutex_unlock(&funnel->tail_lock);
            continue;
        }

        // obtain head lock
        pthread_mutex_lock(&funnel->head_lock);

        // dequeue Link
        if((atomic_load(&(funnel->queue_size))) > 0)
            link = funnel_dequeue(funnel);

        // release head lock
        pthread_mutex_unlock(&funnel->head_lock);

        // operate on Link (add function pointer to struct)
        funnel->func(link->payload);

        // decrement funnel count (this is done to prevent threads from sleeping prematurely)
        atomic_fetch_add(&funnel->queue_size, -1);

        // cleanup
        free(link);
    }
}
