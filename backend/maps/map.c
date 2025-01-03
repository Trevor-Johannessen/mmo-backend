#include "include/map.h"
#include "include/map-definitions.h"

void map_lock_init(){
    int i;

    for(i=0;i<MAP_COUNT;i++){
        pthread_mutex_init(&map_locks[i], 0x0);
    }
}


void map_send_packet(Map *map, Packet *packet, Player *exception){
    Link *link;
    Player *player;

    if(!map || !packet)
        return;
    
    for(link=map->players;link;link=link_next(link)){
        player = (Player *)(link->payload);
        if(player->session && player != exception){
            packet_write(player->session->fd, packet);
        }
    }
}

int map_coord_is_walkable(Map *map, int x, int y){
    int segment;
    segment = map_get_segments(x);
    return !((map->collision[y])->segments[segment] & (0x1<<(x&0x7)));
}

int map_get_segments(int width){
    return width>>3; 
}

int map_enable_coord(Map *map, int x, int y){
    return map_toggle_coord(map, x, y, 0);
}

int map_disable_coord(Map *map, int x, int y){
    return map_toggle_coord(map, x, y, 1);
}

int map_toggle_coord(Map *map, int x, int y, int disable){
    int segment;

    if(!map)
        return 0;

    // check bounds
    if(y >= map->height || y < 0 || x >= map->width || x < 0)
        return 0;

    pthread_mutex_lock(&((map->collision[y])->lock));
    // check if position is valid
    if(disable && !map_coord_is_walkable(map, x, y)){
        pthread_mutex_unlock(&((map->collision[y])->lock));
        return 0;
    }
    // disable position
    segment = map_get_segments(x);
    if(disable)
        (map->collision[y])->segments[segment] |= (1<<(x&0x7));
    else
        (map->collision[y])->segments[segment] &= ~(1<<(x&0x7));
    pthread_mutex_unlock(&((map->collision[y])->lock));
    return 1;
}

long map_random_coord(Map *map){
    int x, y;

    do{
        // generate random coords
        x = rand() % map->width;
        y = rand() % map->height;
    // check that coords are valid
    }while(!map_coord_is_walkable(map, x, y));
    return (long)x<<32 | (y & 0xffffffff);
}

int map_spawn_player(int id, Player *player, int x, int y, int suppress_events){
    Map *map;
    Player *map_player;
    Packet *packet;
    Link *link;
    MoveArgs move_args = {0};

    if(id < 0 || id > MAP_COUNT)
        return 0;

    // get map
    map = map_load(id);

    // send map and move packet
    map_send_map_packet(map, player->session);
    move_args.suppress_events = suppress_events;
    move_args.overlap = 1;
    move_args.map = map;
    player_move(player, x, y, move_args);

    // send all positions of players on map
    for(link=map->players;link;link=link_next(link)){
        map_player = (Player *)link->payload;
        if(map_player != player){
            packet = packet_template_update_position(map_player->id, map_player->x, map_player->y);
            packet_write(player->session->fd, packet);
        }
    }

    return 1;
}

long map_spawn_player_random(int id, Player *player, int suppress_events){
    long pos;
    Map *map;

    if(id < 0 || id > MAP_COUNT)
        return -1;

    // get map
    map = map_load(id);
    player->map = map;

    // get random coordinate
    pos = map_random_coord(player->map);

    // assign player their coordinates
    player->x = pos >> 32;
    player->y = pos & 0xffffffff;

    // add player to map
    map->players = link_add_first(map->players, player);

    // set coordinate to be invalid
    if(!map_disable_coord(map, player->x, player->y)){
        map_unload(id);
        return -1;
    }
    // return coordinates
    return pos;
}

void map_add_player(Map *map, struct player *player){
    player_ref(player);
    map->players = link_add_first(map->players, player);
    player->map = map_load(map->id);
}

void map_remove_player(Map *map, struct player *player){
    Link *link, *prev;
    Packet *packet;
    int removed;
    
    if(!map || !player)
        return;

    // remove player from linked list
    removed = 0;
    prev = 0x0;
    for(link=map->players;link;link=link_next(link)){
        if(link->payload == player){
            if(!prev)
                map->players = link_next(link);
            else
                prev->next = link_next(link);
            player_free(link->payload);
            link_free(link);
            removed = 1;
            break;
        }
        prev = link;
    }
    
    // send packet notifying the player was removed
    packet = packet_template_leaving_map(player->id);
    if(removed)
       for(link=map->players;link;link=link_next(link))
            packet_write(((Player *)(link->payload))->session->fd, packet);
    packet_free(packet);
}

void map_send_map_packet(Map *map, Session *session){
    Packet *packet;

    if(!map || !session)
        return;
    packet = packet_template_map(map->id);
    packet_write(session->fd, packet);
    packet_free(packet);
}

TileRow *map_row_create(int width){
    int mod, segments, row_size;
    TileRow *row;
    segments = map_get_segments(width);
    row_size = sizeof(TileRow)+sizeof(char)*segments;
    row = malloc(row_size);
    memset(row, 0, row_size);
    pthread_mutex_init(&(row->lock), 0x0);
    return row;
}

void map_row_free(int height, TileRow **rows){
    int i;
    for(i=0;i<height;i++){
        pthread_mutex_destroy(&(rows[i]->lock));
        free(rows[i]);
    }
}

Map *map_create(int id, int width, int height){
    Map *map;
    int i;

    map = malloc(sizeof(Map));
    map->id = id;
    map->height = height;
    map->width = width;
    map->players = 0x0;
    map->events = 0x0;
    map->refs = 0;
    pthread_mutex_init(&(map->lock), 0x0);

    // create collision
    map->collision = malloc(sizeof(TileRow *) * height);
    for(i=0;i<height;i++)
        map->collision[i] = map_row_create(width);

    // create decor
    map->decor = malloc(sizeof(char)*height*width);
    memset(map->decor, '-', sizeof(char)*height*width);

    return map;
}

void map_add_event(Map *map, MapEvent *event){
    map->events = link_add_first(map->events, event);
}

MapEvent *map_event_create(int x, int y, void *args, void (*func)(MapEventArgs *)){
    MapEvent *event;

    event = malloc(sizeof(MapEvent));
    event->x = x;
    event->y = y;
    event->func = func;
    event->static_args = args;

    return event;
}

void map_event_free(MapEvent *event){
    if(!event)
        return;
    if(event->static_args)
        free(event->static_args);
    free(event); 
}

void map_event_activate(int x, int y, Map *map, Player *player){
    MapEventArgs *args;
    MapEvent *event;
    Link *link;

    args = map_event_args_create(map_load(map->id), player);
    for(link=map->events;link;link=link_next(link)){
        event = (MapEvent *)link->payload;
        args->event = event;
        if(event->x == x && event->y == y)
            event->func(args);
    }
    map_event_args_free(args);
}

MapEventArgs *map_event_args_create(Map *map, Player *player){
    MapEventArgs *args;

    args = malloc(sizeof(MapEventArgs));
    args->player = player;
    args->map = map;
    args->event = 0x0;
    return args;
}

void map_event_args_free(MapEventArgs *args){
    if(args->map)
        map_unload(args->map->id);
    free(args);
}

void map_free(Map *map){
    Link *link, *parent;

    // free decor
    free(map->decor);

    // free rows
    map_row_free(map->height, map->collision);

    // free players
    link = map->players;
    while(link){
        parent = link;
        link = link=link_next(link);
        player_free(parent->payload);
        free(parent);
    }

    // free events
    link = map->events;
    while(link){
        parent = link;
        link = link_next(link);
        map_event_free(parent->payload);
        free(parent);
    }
    free(map);
}

Map *map_load(int id){
    if(id > MAP_COUNT || id < 0)
        return 0x0;
    // check if map is not already initalize (is 0x0)
    pthread_mutex_lock(&map_locks[id]);
    if(!tiles[id]){
        tiles[id] = map_loaders[id]();    
    }
    tiles[id]->refs++;
    pthread_mutex_unlock(&map_locks[id]);
    return tiles[id];
}

void map_unload(int id){
    if(id > MAP_COUNT || id < 0)
        return;
    if(!tiles[id])
        return;
    pthread_mutex_lock(&map_locks[id]);
    tiles[id]->refs--;
    if(!tiles[id]->refs){
        map_free(tiles[id]);
        tiles[id] = 0x0;
    }
    pthread_mutex_unlock(&map_locks[id]);
}

int map_check_bounds(Map *map, int x, int y){
    return x >= 0 && x < map->width && y >= 0 && y < map->height;
}