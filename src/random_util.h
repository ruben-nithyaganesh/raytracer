#ifndef R_UTIL
#define R_UTIL

#include <stdlib.h>
#include <math.h>

inline double degrees_to_radians(double degrees) {
    return degrees * 3.14 / 180.0;
}

double my_random_double() {
    return rand() / (RAND_MAX + 1.0);
}

double my_random_double_between(double min, double max) {
    return min + (max - min) * my_random_double();
}

int random_int_between(int start, int end) {
    if(start >= end) return start;
    int diff = end - start;
    return (rand() % (diff) + start);
}

#endif