#include "include/db.h"

char *db_get_name(char *id){
    const int BUF_SIZE = 100;
    char *name;
    name = malloc(BUF_SIZE);
    memset(name, 0, BUF_SIZE);
    strcpy(name, "Sam");
    return name;
}