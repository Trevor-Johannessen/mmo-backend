#include "include/funnel.h"

Funnel *funnel_create(){
    Funnel *funnel;
    pthread_mutexattr_t attr;
    int i;

    if(!(funnel = malloc(sizeof(Funnel))))
        return 0x0;

    // initalize
    funnel->head = 0x0;
    funnel->tail = 0x0;
    funnel->active = 1;
    funnel->enabled = 1;
    funnel->thread_count = ATOMIC_VAR_INIT(0);
    funnel->queue_size = ATOMIC_VAR_INIT(0);
    funnel->func = 0x0;
    funnel->free_payload_func = 0x0;
    pthread_mutex_init(&funnel->head_lock, 0x0);
    pthread_mutex_init(&funnel->tail_lock, 0x0);
    pthread_mutex_init(&funnel->thread_lock, 0x0);

    for(i=0;i<MAX_FUNNEL_THREADS;i++)
        funnel->tids[i] = 0x0;
    pthread_create(&funnel->tids[0], 0x0, funnel_thread_start, funnel);

    return funnel;
}

void funnel_free(Funnel *funnel, int drop_payloads){
    int i;

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

    // disable threads
    for(i=0;i<MAX_FUNNEL_THREADS;i++)
        funnel->tids[i] = 0;

    // wait for threads to finish
    pthread_mutex_unlock(&funnel->head_lock);
    pthread_mutex_unlock(&funnel->tail_lock);
    pthread_mutex_unlock(&funnel->thread_lock);

    
    // free struct
    pthread_mutex_destroy(&funnel->head_lock);
    pthread_mutex_destroy(&funnel->tail_lock);
    pthread_mutex_destroy(&funnel->thread_lock);
    // does atomic_int need to be freed?
    free(funnel);

}

void funnel_enqueue(Funnel *funnel, void *payload){
    int grabbed_head_mutex;
    Link *link;

    // create link
    link = link_create();
    if(!link)
        return;
    link->payload = payload;

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

    // try to expand
    funnel_expand(funnel);
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
    atomic_fetch_add(&funnel->queue_size, -1);

    // release locks
    pthread_mutex_unlock(&funnel->head_lock);
    if(grabbed_tail_mutex)
        pthread_mutex_unlock(&funnel->tail_lock);

    // try to contract
    funnel_contract(funnel);

    // return head
    return out;
}

void funnel_expand(Funnel *funnel){
    // check size
    if(atomic_load(&funnel->queue_size) < atomic_load(&funnel->thread_count) * funnel->new_thread_interval)
        return;

    // grab head and thread lock
    pthread_mutex_lock(&funnel->head_lock);
    pthread_mutex_lock(&funnel->thread_lock);

    // check size
    if(atomic_load(&funnel->thread_count) == MAX_FUNNEL_THREADS)
        goto funnel_expand_cleanup;
    if(atomic_load(&funnel->queue_size) >= atomic_load(&funnel->thread_count) * funnel->new_thread_interval){
        // spin new thread
        pthread_create(&funnel->tids[atomic_load(&funnel->thread_count)], 0x0, funnel_thread_start, (void *)funnel);
        atomic_fetch_add(&funnel->thread_count, 1);
    }

    // release locks
    funnel_expand_cleanup:
    pthread_mutex_unlock(&funnel->head_lock);
    pthread_mutex_unlock(&funnel->thread_lock);
}

void funnel_contract(Funnel *funnel){
    pthread_t tid;

    // check size
    if(atomic_load(&funnel->queue_size) > (atomic_load(&funnel->thread_count)-2) * funnel->new_thread_interval)
        return;

    // grab tail and thread lock
    pthread_mutex_lock(&funnel->tail_lock);
    pthread_mutex_lock(&funnel->thread_lock);

    // check size and remove thread
    if(atomic_load(&funnel->queue_size) <= (atomic_load(&funnel->thread_count)-2) * funnel->new_thread_interval){
        tid = funnel->tids[atomic_load(&funnel->thread_count)-1];
        funnel->tids[atomic_load(&funnel->thread_count)-1] = 0;
    }
    if(!atomic_load(&funnel->queue_size)){
        tid = funnel->tids[0];
        funnel->tids[0] = 0;
    }

    // release locks
    pthread_mutex_unlock(&funnel->tail_lock);
    pthread_mutex_unlock(&funnel->thread_lock);

    pthread_join(tid, 0x0); // this gives more accurate unit tests, can be disabled.
}

void *funnel_thread_start(void *funnel_in){
    Funnel *funnel;
    int tid_index, i;
    pthread_t my_tid;
    Link *link;

    funnel = (Funnel *)funnel_in;

    // detatch self
    pthread_detach(pthread_self());

    // find tid index in array
    my_tid = pthread_self();
    tid_index = -1;
    while(tid_index < 0){ // need to spin until pthread_create assigns the tid to the array
        for(i=0;i<MAX_FUNNEL_THREADS;i++)
            if(funnel->tids[i] == my_tid)
                tid_index = i;
    }

    while(funnel->tids[tid_index] == my_tid){
        if(!funnel->active || !funnel->enabled)
            continue;
        
        // dequeue Link
        if(!funnel->func) // don't dequeue if you can't act on the node
            continue;
        if(!(link = funnel_dequeue(funnel)))
            continue;

        // operate on Link (add function pointer to struct)
        funnel->func(link->payload);
        free(link);
    }
    atomic_fetch_add(&funnel->thread_count, -1);
}
