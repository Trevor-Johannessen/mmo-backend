#include <criterion/criterion.h>
#include <pthread.h>
#include "../db/include/db.h"
#include "../db/include/db-player.h"
#include "../player/include/player.h"

Test(db, test_db_connect) {
    MongoConnection *conn;

    mongoc_init();
    conn = db_connect();
    db_free(conn);
    mongoc_cleanup();
}

Test(db, test_db_get_player) {
    Player *player;
    MongoConnection *conn;

    mongoc_init();
    conn = db_connect();

    // Find player that exists.
    player = db_player_get_player(conn, "aaaaaa");
    cr_assert(player, "Player with id 'aaaaaa' not returned.");
    player_print(player);

    // Find player that does not exist.
    player = db_player_get_player(conn, "aaaaab");
    cr_assert(!player, "Invalid player with id 'aaaaab' returned.");

    db_free(conn);
    mongoc_cleanup();
}