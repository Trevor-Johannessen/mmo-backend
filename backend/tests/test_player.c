#include <criterion/criterion.h>
#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include "../player/include/movement.h"
#include "../player/include/player.h"
#include "../maps/include/map.h"


void *move_player_thread(void *player){
    player_move((Player *)player, 5, 4);
}

Test(player, test_player_create) {
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
    map_spawn_player(0, player, 4, 4);
    player_free(player);
}

Test(player, test_player_move) {
    Player *sam, *cid;
    int i;
    void *trash;
    pthread_t tid1, tid2;

    // set up player 1
    sam = player_create();
    sam->name = malloc(4);
    memset(sam->name, 0, 4);
    strncpy(sam->name, "Sam", 3);
    sam->max_move = 1;  
    map_spawn_player(0, sam, 4, 4);

    // set up player 2
    cid = player_create();
    cid->name = malloc(4);
    memset(cid->name, 0, 4);
    strncpy(cid->name, "Cid", 3);
    cid->max_move = 1;   
    map_spawn_player(0, cid, 6, 4);

    // test basic movement
    cr_assert_not(map_coord_is_walkable(sam->map, 4, 4), "The source tile is walkable before player has attempted movement.");
    cr_assert(map_coord_is_walkable(sam->map, 5, 4), "The destination tile is not walkable before player has attempted movement.");
    player_move(sam, 5, 4);
    cr_assert(map_coord_is_walkable(sam->map, 4, 4), "The source tile is not walkable after player has attempted movement.");
    cr_assert_not(map_coord_is_walkable(sam->map, 5, 4), "The destination tile is walkable after player has attempted movement.");
    player_move(sam, 4, 4);
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
        player_move(sam, 4, 4);
        player_move(cid, 6, 4);
    }

    player_free(sam);
    player_free(cid);
}