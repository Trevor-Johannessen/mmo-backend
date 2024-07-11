#include <criterion/criterion.h>
#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include "../player/include/movement.h"
#include "../player/include/player.h"
#include "../maps/include/map.h"
#include "../db/include/db.h"
#include "../db/include/db-player.h"

MongoConnection *GLOBAL_CONNECTION;

void *move_player_thread(void *player){
    MoveArgs args = {0};
    player_move((Player *)player, 5, 4, args);
}

void test_player_init_global(){
    GLOBAL_CONNECTION = db_connect();
}

void test_player_destroy_global(){
    db_free(GLOBAL_CONNECTION);
}

Test(player, test_player_create, .init = test_player_init_global, .fini = test_player_destroy_global) {
    Player *player;
    // test uninitalized player
    player = player_create();
    player_free(player);

    // test initalized player
    player = player_create();
    player->name = malloc(5);
    memset(player->name, 0, 4);
    strncpy(player->name, "Sam", 3);
    player->max_move = 1;  
    map_spawn_player(0, player, 4, 4, 0);
    player_free(player);
}

Test(player, test_player_move, .init = test_player_init_global, .fini = test_player_destroy_global) {
    Player *sam, *cid;
    int i;
    void *trash;
    pthread_t tid1, tid2;
    MoveArgs move_args = {0};

    // set up player 1
    sam = player_create();
    sam->name = malloc(4);
    memset(sam->name, 0, 4);
    strncpy(sam->name, "Sam", 3);
    sam->max_move = 1;  
    map_spawn_player(0, sam, 4, 4, 0);

    // set up player 2
    cid = player_create();
    cid->name = malloc(4);
    memset(cid->name, 0, 4);
    strncpy(cid->name, "Cid", 3);
    cid->max_move = 1;   
    map_spawn_player(0, cid, 6, 4, 0);

    // test basic movement
    cr_assert_not(map_coord_is_walkable(sam->map, 4, 4), "The source tile is walkable before player has attempted movement.");
    cr_assert(map_coord_is_walkable(sam->map, 5, 4), "The destination tile is not walkable before player has attempted movement.");
    player_move(sam, 5, 4, move_args);
    cr_assert(map_coord_is_walkable(sam->map, 4, 4), "The source tile is not walkable after player has attempted movement.");
    cr_assert_not(map_coord_is_walkable(sam->map, 5, 4), "The destination tile is walkable after player has attempted movement.");
    player_move(sam, 4, 4, move_args);
    cr_assert_not(map_coord_is_walkable(sam->map, 4, 4), "The source tile is walkable after player has moved back.");
    cr_assert(map_coord_is_walkable(sam->map, 5, 4), "The destination tile is not walkable after player has moved back.");

    // test contested movement
    for(i=0;i<10000;i++){
        // check if spot is valid
        cr_assert(map_coord_is_walkable(sam->map, 5, 4), "The requested tile is not walkable before players have attempted movement. (Iteration %d)", i+1);

        // move players to same spot
        pthread_create(&tid1, 0x0, move_player_thread, sam);
        pthread_create(&tid2, 0x0, move_player_thread, cid);
        pthread_join(tid1, trash);
        pthread_join(tid2, trash);

        // check if spot is valid
        cr_assert_not(map_coord_is_walkable(sam->map, 5, 4), "The requested tile is walkable after players have attempted movement.");

        // check that players positions are not equal
        cr_assert_neq(sam->x, cid->x, "Players are overlapping.");

        // reset players positions
        player_move(sam, 4, 4, move_args);
        player_move(cid, 6, 4, move_args);
    }

    player_free(sam);
    player_free(cid);
}

Test(player, test_player_db, .init = test_player_init_global, .fini = test_player_destroy_global) {
    Player *p1;

    // Pull player from db
    p1 = db_player_get_player(GLOBAL_CONNECTION, "unit_test");
    cr_assert(p1, "P1 returned null.");

    // check that it has valid properties
    cr_assert_not(strcmp(p1->name, "unit_tester"), "Player name is incorrect. (Expected \"unit_tester\", Got %s)", p1->name);
    cr_assert_eq(p1->x, 5, "Player X position is incorrect. (Expected 5, Got %d)", p1->x);
    cr_assert_eq(p1->y, 7, "Player Y position is incorrect. (Expected 7, Got %d)", p1->y);
    cr_assert_eq(p1->max_move, 1, "Player max move is incorrect. (Expected 1, Got %d)", p1->max_move);

    // change properties
    p1->x = 1;
    p1->y = 1;
    p1->max_move = 2;
    player_change_name(p1, "tommy");
    p1->modified = 1;
    // TODO: ADD MAP CHANGE HERE

    // free player
    player_free(p1);
    p1=0x0;

    // pull same player from db
    p1 = db_player_get_player(GLOBAL_CONNECTION, "unit_test");
    cr_assert(p1, "P1 returned null.");

    // check that it has changed properties
    cr_assert_not(strcmp(p1->name, "tommy"), "Player name is incorrect. (Expected \"tommy\", Got %s)", p1->name);
    cr_assert_eq(p1->x, 1, "Player X position is incorrect. (Expected 1, Got %d)", p1->x);
    cr_assert_eq(p1->y, 1, "Player Y position is incorrect. (Expected 1, Got %d)", p1->y);
    cr_assert_eq(p1->max_move, 2, "Player max move is incorrect. (Expected 2, Got %d)", p1->max_move);

    // revert properties
    p1->x = 5;
    p1->y = 7;
    p1->max_move = 1;
    player_change_name(p1, "unit_tester");
    p1->modified = 1;

    // free player
    player_free(p1);
}

Test(player, test_player_cache, .init = player_cache_init, .fini = player_cache_destroy) {
    // Create new player
    Player *p1, *p2, *p3;
    p3 = player_create();
    p3->x=5;
    p3->x=7;
    p3->refs=1;
    p3->id = malloc(5);
    strcpy(p3->id, "test");

    // Insert player into cache
    cr_assert(player_cache_insert(p3->id, p3), "Could not insert p3 into player cache.");

    // Pull same player from cache
    p1 = player_cache_find("test");
    p2 = player_cache_find("test");

    // check if addresses are equal
    cr_assert_eq(p3, p1, "Memory addresses retrieved from cache are different. (p3!=p1)");
    cr_assert_eq(p1, p2, "Memory addresses retrieved from cache are different. (p1!=p2)");

    // check ref == 2
    cr_assert_eq(p3->refs, 3, "Invalid ref count. (Expected 3, Got %d)", p3->refs);

    // Free players
    player_free(p1);
    player_free(p2);
    player_free(p3);

    // Check that player is no longer in cache
    p1=0x0;
    p1 = player_cache_find("test");
    cr_assert(!p1, "Found player in cache when that player should have been freed.");
}

// Test(player, test_player_concurrent) {
//     // ref player a ton of times

//     // unref player a ton of times

//     // concurrently increment or decrement property

// }