#include <criterion/criterion.h>
#include <pthread.h>
#include<unistd.h>
#include "../structures/include/funnel.h"
#include "../structures/include/linked-list.h"

void funnel_enqueue_dummy(Funnel *funnel, int n){
    int i;

    for(i=0;i<n;i++)
        funnel_enqueue(funnel, 0x0);
}

void funnel_dequeue_dummy(Funnel *funnel, int n){
    int i;

    for(i=0;i<n;i++)
        funnel_dequeue(funnel);
    sleep(1);
}

void *funnel_enqueue_thread(void *funnel){
    int i;
    for(i=0;i<1000;i++)
        funnel_enqueue((Funnel *)funnel, 0x0);
}

void *funnel_dequeue_thread(void *funnel){
    int i;
    pthread_t pid;
    pid = pthread_self();
    for(i=0;i<1000;i++){
        funnel_dequeue((Funnel *)funnel);
    }
}

// create and destroy a funnel
Test(funnel, test_funnel_create) {
    Funnel *funnel;
    Link *link, *head;
    int i;
    funnel = 0x0;

    // create and destroy
    funnel = funnel_create();
    cr_assert(funnel, "Funnel creation failed, funnel_create returned null.");
    funnel_free(funnel, 0);

    // create and destroy with payloads (Don't enqueue or it will start doing stuff)
    funnel = funnel_create();
    funnel->free_payload_func = (void (*)(void *))link_free;
    head = link_create();
    for(i=0;i<5;i++) // funnel thread will not execute loop body since funnel->active = 0
        head = link_add_first(head, 0x0);
    funnel_free(funnel, 1); // use valgrind to check for leak
}


// test adding a bunch of items with a low threshold and count the thread_count and tid array
// be sure to also check teardown
Test(funnel, test_funnel_threshold){
    Funnel *funnel;
    const int retry_count = 3, retry_sleep = 3;
    int i;

    funnel = funnel_create();
    cr_assert(funnel, "Funnel creation failed, funnel_create returned null.");
    funnel->new_thread_interval = 3;
    funnel->enabled = 0;
    cr_assert_eq(funnel->queue_size, 0, "Invalid queue size. (Expected 0, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 0, "Invalid thread count. (Expected 0, got %d)", funnel->thread_count);

    // ENQUEUE
    funnel_enqueue_dummy(funnel, 2);
    cr_assert_eq(funnel->queue_size, 2, "Invalid queue size. (Expected 2, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 2, "Invalid thread count. (Expected 1, got %d)", funnel->thread_count);

    funnel_enqueue_dummy(funnel, 1);
    cr_assert_eq(funnel->queue_size, 3, "Invalid queue size. (Expected 3, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 2, "Invalid thread count. (Expected 2, got %d)", funnel->thread_count);

    funnel_enqueue_dummy(funnel, 3);
    cr_assert_eq(funnel->queue_size, 6, "Invalid queue size. (Expected 6, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 3, "Invalid thread count. (Expected 3, got %d)", funnel->thread_count);

    funnel_enqueue_dummy(funnel, 4);
    cr_assert_eq(funnel->queue_size, 10, "Invalid queue size. (Expected 10, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 4, "Invalid thread count. (Expected 4, got %d)", funnel->thread_count);

    funnel_enqueue_dummy(funnel, 10);
    cr_assert_eq(funnel->queue_size, 20, "Invalid queue size. (Expected 10, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 7, "Invalid thread count. (Expected 7, got %d)", funnel->thread_count);

    // DEQUEUE (need sleeps to give threads enough time to close)
    funnel_dequeue_dummy(funnel, 2);
    cr_assert_eq(funnel->queue_size, 18, "Invalid queue size. (Expected 18, got %d)", funnel->queue_size);
    for(i=0;i<retry_count;i++) if(funnel->thread_count != 7) sleep(retry_sleep);
    cr_assert_eq(funnel->thread_count, 7, "Invalid thread count. (Expected 7, got %d)", funnel->thread_count);

    funnel_dequeue_dummy(funnel, 3);
    cr_assert_eq(funnel->queue_size, 15, "Invalid queue size. (Expected 15, got %d)", funnel->queue_size);
    for(i=0;i<retry_count;i++) if(funnel->thread_count != 6) sleep(retry_sleep);
    cr_assert_eq(funnel->thread_count, 6, "Invalid thread count. (Expected 6, got %d)", funnel->thread_count);

    funnel_dequeue_dummy(funnel, 3);
    cr_assert_eq(funnel->queue_size, 12, "Invalid queue size. (Expected 12, got %d)", funnel->queue_size);
    for(i=0;i<retry_count;i++) if(funnel->thread_count != 5) sleep(retry_sleep);
    cr_assert_eq(funnel->thread_count, 5, "Invalid thread count. (Expected 5, got %d)", funnel->thread_count);

    funnel_dequeue_dummy(funnel, 9);
    cr_assert_eq(funnel->queue_size, 3, "Invalid queue size. (Expected 3, got %d)", funnel->queue_size);
    for(i=0;i<retry_count;i++) if(funnel->thread_count != 2) sleep(retry_sleep);
    cr_assert_eq(funnel->thread_count, 2, "Invalid thread count. (Expected 2, got %d)", funnel->thread_count);

    funnel_dequeue_dummy(funnel, 3);
    cr_assert_eq(funnel->queue_size, 0, "Invalid queue size. (Expected 0, got %d)", funnel->queue_size);
    for(i=0;i<retry_count;i++) if(funnel->thread_count != 0) sleep(retry_sleep);
    cr_assert_eq(funnel->thread_count, 0, "Invalid thread count. (Expected 0, got %d)", funnel->thread_count);

    funnel_free(funnel, 0);
}



// spin a bunch of threads adding stuff and make sure count is correct
Test(funnel, test_funnel_concurrent){
    int i, expected_threads;
    const int retry_count = 1, retry_sleep = 1;
    pthread_t tids[20];
    Funnel *funnel;

    funnel = funnel_create();
    cr_assert(funnel, "Funnel creation failed, funnel_create returned null.");
    funnel->new_thread_interval = 1500;
    funnel->enabled = 0;
    funnel->free_payload_func = (void (*)(void *))link_free;
    cr_assert_eq(funnel->queue_size, 0, "Invalid queue size. (Expected 0, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 0, "Invalid thread count. (Expected 0, got %d)", funnel->thread_count);

    // concurrent enqueues
    for(i=0;i<10;i++)
        pthread_create(&tids[i], 0x0, funnel_enqueue_thread, funnel);
    for(i=0;i<10;i++)
        pthread_join(tids[i], 0x0);
    cr_assert_eq(funnel->queue_size, 10000, "Invalid queue size. (Expected 10,000, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 7, "Invalid thread count. (Expected 7, got %d)", funnel->thread_count);

    // concurrent dequeues
    for(i=0;i<10;i++){
        pthread_create(&tids[i], 0x0, funnel_dequeue_thread, funnel);
    }
    for(i=0;i<10;i++){
        pthread_join(tids[i], 0x0);
    }
    //for(i=0;i<retry_count;i++) if(funnel->thread_count) sleep(3);
    cr_assert_eq(funnel->queue_size, 0, "Invalid queue size. (Expected 0, got %d)", funnel->queue_size);
    cr_assert_eq(funnel->thread_count, 0, "Invalid thread count. (Expected 0, got %d)", funnel->thread_count);

    // a mess
    // enqueues
    for(i=0;i<10;i++)
        pthread_create(&tids[i], 0x0, funnel_enqueue_thread, funnel);
    // dequeue
    for(i=10;i<20;i++)
        pthread_create(&tids[i], 0x0, funnel_dequeue_thread, funnel);
    for(i=0;i<20;i++)
        pthread_join(tids[i], 0x0);
    sleep(1); // wait for all downstream threads to finish

    expected_threads = funnel->queue_size / funnel->new_thread_interval + 1;
    if(!funnel->queue_size)
        expected_threads = 0;
    if((funnel->queue_size < funnel->new_thread_interval) && (funnel->thread_count == 2))
        expected_threads = 2;
    cr_assert_eq(funnel->thread_count, expected_threads, "Invalid thread count. (Expected %d, got %d)", expected_threads, funnel->thread_count);

    funnel_free(funnel, 1);
}

