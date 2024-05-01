#include <criterion/criterion.h>
#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include "../maps/include/tile.h"

atomic_int disable_count;

typedef struct {
    int x;
    int y;
    Tile *tile;
} disableCoordArgs;

void *tile_disable_coord_thread(void *buf){
    int val;

    val = tile_disable_coord(((disableCoordArgs *)buf)->tile, ((disableCoordArgs *)buf)->x, ((disableCoordArgs *)buf)->y);
    if(val)
        atomic_fetch_add(&disable_count, 1);
    return 0x0;
}

void *tile_ref_thread(void *args){
    tile_load(0);
}

void *tile_unref_thread(void *args){
    tile_unload(0);
}

Test(tiles, test_tile_get_segment){
    int trials[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 15, 16, 23, 24, 31, 3624, 3631, 3632};
    int answers[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 453, 453, 454};
    int i, n;

    for(i=0;i<(sizeof(trials)/sizeof(int));i++){
        n = tile_get_segments(trials[i]);
        cr_assert_eq(n, answers[i], "Incorrect segment for x=%d. (Expected %d, Got %d)", trials[i], n, answers[i]);
    }
}

Test(tiles, test_tile_disable) {
    Tile *tile;
    int i, key;

    tile = tile_create(0, 64, 64);

    // test when only bit in char
    key=1;
    for(i=0;i<8;i++){
        tile_disable_coord(tile, i, 0);
        cr_assert_eq((tile->collision[0])->segments[0], key, "Disable segment invalid. (Expected %d, Got %d)", (tile->collision[0])->segments[0], key);
        tile_enable_coord(tile, i, 0);
        cr_assert_eq((tile->collision[0])->segments[0], 0, "Enable segment invalid. (Expected %d, Got %d)", (tile->collision[0])->segments[0], 0);
        key = key << 1;
    }

    // test when other bits in char
    key=1;
    for(i=0;i<8;i++){
        tile_disable_coord(tile, i, 0);
        cr_assert_eq((tile->collision[0])->segments[0], key, "Disable segment invalid. (Expected %d, Got %d)", (tile->collision[0])->segments[0], key);
        key = (key << 1)+1;
    }
}

Test(tiles, test_tile_create){
    Tile *tiles[10000];
    TileRow *row;
    int i, j, t, segments, width=64, height=64;

    segments = tile_get_segments(width-1);
    for(t=0;t<10000;t++){
        tiles[t] = tile_create(8, width, height);
        cr_assert_eq(tiles[t]->id, 8, "Tile ID is invalid. (Expected 8, Got %d)", tiles[t]->id);
        for(i=0;i<height;i++){
            row = tiles[t]->collision[i];
            for(j=0; j<segments; j++)
                cr_assert_eq(row->segments[j], 0, "Row %d, segment %d is invalid. (Expected 0, Got %d)", i, j, row->segments[j]);
        }
    }
    for(t=0;t<10000;t++)
        tile_free(tiles[t]);
    
}

Test(tiles, test_tile_occupy_position) {
    long pos;
    int i, iterations, val;
    pthread_t  tid1, tid2;
    disableCoordArgs args;

    // setup
    args.tile = tile_loaders[0]();

    // test if return value is correct
    val = tile_disable_coord(args.tile, 0, 0);
    cr_assert_eq(val, 1, "Disable coord when free failed. (Expecting 1, got %d)", val);
    cr_assert_eq(tile_coord_is_empty(args.tile, 0, 0), 0, "Disable coord did not set coord as empty.");
    val = tile_disable_coord(args.tile, 0, 0);
    cr_assert_eq(val, 0, "Disable coord when occupied failed. (Expecting 0, got %d)", val);
    tile_enable_coord(args.tile, 0, 0);

    // test for race condition
    disable_count = ATOMIC_VAR_INIT(0);;
    iterations = 10000;
    for(i=0; i<iterations; i++){
        pos = tile_random_coord(args.tile);
        args.x = pos >> 32;
        args.y = pos & 0xffffffff;
        pthread_create(&tid1, 0x0, tile_disable_coord_thread, &args);
        pthread_create(&tid2, 0x0, tile_disable_coord_thread, &args);
        pthread_join(tid1, 0x0);
        pthread_join(tid2, 0x0);
        tile_enable_coord(args.tile, args.x, args.y);
    }
    cr_assert_eq(atomic_load(&disable_count), iterations, "Iterations invalid. (Expected %d, got %d)", iterations, disable_count);
}

Test(tiles, test_tile_ref) {
    Tile *tile;
    int i;
    pthread_t tid1, tid2;

    tile_unload(0);
    cr_assert_eq(tile_load(-1), 0x0, "Loading invalid tile attempted to load tile.");
    tile_unload(-1);

    tile_load(0);
    cr_assert_neq(tiles[0], 0x0, "Tile 0 is null after being initalized.");
    tile_unload(0);
    cr_assert_eq(tiles[0], 0x0, "Tile 0 is not null after being freed.");
    
    // check constant reffing
    for(i=0;i<10000;i++)
        pthread_create(&tid1, 0x0, tile_ref_thread, 0x0);
    tile = tile_load(0);
    cr_assert_eq(tile->refs, 10001, "Tile has invalid number of refs. (Expected 10001, Got %d)", tile->refs);

    // check constant unrefing
    for(i=0;i<10000;i++)
        pthread_create(&tid1, 0x0, tile_unref_thread, 0x0);
    cr_assert_eq(tile->refs, 1, "Tile has invalid number of refs. (Expected 1, Got %d)", tile->refs);
    tile_unload(0);

    // check contention
    for(i=0;i<10000;i++){
        pthread_create(&tid1, 0x0, tile_ref_thread, 0x0);
        pthread_create(&tid2, 0x0, tile_unref_thread, 0x0);
    }
    tile = tile_load(0);
    cr_assert_eq(tile->refs, 1, "Tile has invalid number of refs. (Expected 1, Got %d)", tile->refs);





}