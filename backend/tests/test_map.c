#include <criterion/criterion.h>
#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include "../maps/include/map.h"

atomic_int disable_count;

typedef struct {
    int x;
    int y;
    Map *map;
} disableCoordArgs;

void *map_disable_coord_thread(void *buf){
    int val;

    val = map_disable_coord(((disableCoordArgs *)buf)->map, ((disableCoordArgs *)buf)->x, ((disableCoordArgs *)buf)->y);
    if(val)
        atomic_fetch_add(&disable_count, 1);
    return 0x0;
}

void *map_ref_thread(void *args){
    map_load(0);
}

void *map_unref_thread(void *args){
    map_unload(0);
}

Test(tiles, test_map_get_segment){
    int trials[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 15, 16, 23, 24, 31, 3624, 3631, 3632};
    int answers[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 453, 453, 454};
    int i, n;

    for(i=0;i<(sizeof(trials)/sizeof(int));i++){
        n = map_get_segments(trials[i]);
        cr_assert_eq(n, answers[i], "Incorrect segment for x=%d. (Expected %d, Got %d)", trials[i], n, answers[i]);
    }
}

Test(tiles, test_map_disable) {
    Map *map;
    int i, key;

    map = map_create(0, 64, 64);

    // test when only bit in char
    key=1;
    for(i=0;i<8;i++){
        map_disable_coord(map, i, 0);
        cr_assert_eq((map->collision[0])->segments[0], key, "Disable segment invalid. (Expected %d, Got %d)", (map->collision[0])->segments[0], key);
        map_enable_coord(map, i, 0);
        cr_assert_eq((map->collision[0])->segments[0], 0, "Enable segment invalid. (Expected %d, Got %d)", (map->collision[0])->segments[0], 0);
        key = key << 1;
    }

    // test when other bits in char
    key=1;
    for(i=0;i<8;i++){
        map_disable_coord(map, i, 0);
        cr_assert_eq((map->collision[0])->segments[0], key, "Disable segment invalid. (Expected %d, Got %d)", (map->collision[0])->segments[0], key);
        key = (key << 1)+1;
    }
}

Test(tiles, test_map_create){
    Map *tiles[10000];
    TileRow *row;
    int i, j, t, segments, width=64, height=64;

    segments = map_get_segments(width-1);
    for(t=0;t<10000;t++){
        tiles[t] = map_create(8, width, height);
        cr_assert_eq(tiles[t]->id, 8, "Map ID is invalid. (Expected 8, Got %d)", tiles[t]->id);
        for(i=0;i<height;i++){
            row = tiles[t]->collision[i];
            for(j=0; j<segments; j++)
                cr_assert_eq(row->segments[j], 0, "Row %d, segment %d is invalid. (Expected 0, Got %d)", i, j, row->segments[j]);
        }
    }
    for(t=0;t<10000;t++)
        map_free(tiles[t]);
    
}

Test(tiles, test_map_occupy_position) {
    long pos;
    int i, iterations, val;
    pthread_t  tid1, tid2;
    disableCoordArgs args;

    // setup
    args.map = map_loaders[0]();

    // test if return value is correct
    val = map_disable_coord(args.map, 0, 0);
    cr_assert_eq(val, 1, "Disable coord when free failed. (Expecting 1, got %d)", val);
    cr_assert_eq(map_coord_is_walkable(args.map, 0, 0), 0, "Disable coord did not set coord as empty.");
    val = map_disable_coord(args.map, 0, 0);
    cr_assert_eq(val, 0, "Disable coord when occupied failed. (Expecting 0, got %d)", val);
    map_enable_coord(args.map, 0, 0);

    // test for race condition
    disable_count = ATOMIC_VAR_INIT(0);;
    iterations = 10000;
    for(i=0; i<iterations; i++){
        pos = map_random_coord(args.map);
        args.x = pos >> 32;
        args.y = pos & 0xffffffff;
        pthread_create(&tid1, 0x0, map_disable_coord_thread, &args);
        pthread_create(&tid2, 0x0, map_disable_coord_thread, &args);
        pthread_join(tid1, 0x0);
        pthread_join(tid2, 0x0);
        map_enable_coord(args.map, args.x, args.y);
    }
    cr_assert_eq(atomic_load(&disable_count), iterations, "Iterations invalid. (Expected %d, got %d)", iterations, disable_count);
}

Test(tiles, test_map_ref) {
    Map *map, *map2;
    int i;
    pthread_t tid_arr[10000], tid_arr_2[10000];
    void *trash;

    map_unload(0);
    cr_assert_eq(map_load(-1), 0x0, "Loading invalid map attempted to load map.");
    map_unload(-1);

    map_load(0);
    cr_assert_neq(tiles[0], 0x0, "Map 0 is null after being initalized.");
    map_unload(0);
    cr_assert_eq(tiles[0], 0x0, "Map 0 is not null after being freed.");
    
    // check constant reffing
    for(i=0;i<10000;i++)
        pthread_create(&(tid_arr[i]), 0x0, map_ref_thread, 0x0);
    for(i=0;i<10000;i++)
        pthread_join(tid_arr[i], trash);
    map = map_load(0);
    cr_assert_eq(map->refs, 10001, "Map has invalid number of refs. (Expected 10001, Got %d)", map->refs);

    // check constant unrefing
    for(i=0;i<10000;i++)
        pthread_create(&(tid_arr[i]), 0x0, map_unref_thread, 0x0);
    for(i=0;i<10000;i++)
        pthread_join(tid_arr[i], trash);
    cr_assert_eq(map->refs, 1, "Map has invalid number of refs. (Expected 1, Got %d)", map->refs);
}