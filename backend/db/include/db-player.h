#ifndef DBPLAYER_H
#define DBPLAYER_H

#include "db.h"
#include "../../maps/include/map.h"
//#include "../../player/include/player.h"

struct player *db_player_get_player(MongoConnection *conn, char *id);
void db_player_write_player(MongoConnection *conn, struct player *player);

#endif