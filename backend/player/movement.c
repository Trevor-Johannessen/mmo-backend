#include "include/movement.h"

int movement_check_valid(int max, int x1, int y1, int x2, int y2){
    return max >= movement_count_distance(x1, y1, x2, y2);
}

int movement_count_distance(int x1, int y1, int x2, int y2){
    int x_dist = x1 > x2 ? x1-x2 : x2-x1;
    int y_dist = y1 > y2 ? y1-y2 : y2-y1;
    return x_dist + y_dist;
}