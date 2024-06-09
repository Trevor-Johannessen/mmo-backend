#ifndef DBPLAYER_H
#define DBPLAYER_H

#include "db.h"
#include "../../player/include/player.h"
#include "../../maps/include/map.h"

Player *db_player_get_player(MongoConnection *conn, char *id);


#endif