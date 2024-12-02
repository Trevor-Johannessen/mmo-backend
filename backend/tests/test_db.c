#include <criterion/criterion.h>
#include <pthread.h>
#include "../db/include/db.h"
#include "../db/include/db-player.h"
#include "../player/include/player.h"

extern MongoConnection *GLOBAL_CONNECTION;

void test_db_init_global(){
    GLOBAL_CONNECTION = db_connect();
    player_cache_init();
}

void test_db_destroy_global(){
    db_free(GLOBAL_CONNECTION);
}

Test(db, test_db_connect) {
    MongoConnection *conn;

    mongoc_init();
    conn = db_connect();
    db_free(conn);
    mongoc_cleanup();
}

Test(db, test_db_get_player, .init = test_db_init_global, .fini = test_db_destroy_global) {
    Player *player;
    MongoConnection *conn;

    mongoc_init();
    conn = db_connect();

    // Find player that exists.
    player = db_player_get_player(conn, "test");
    cr_assert(player, "Player with id 'test' not returned.");
    player_print(player);

    // Find player that does not exist.
    player = db_player_get_player(conn, "testb");
    cr_assert(!player, "Invalid player with id 'testb' returned.");

    db_free(conn);
    mongoc_cleanup();
}