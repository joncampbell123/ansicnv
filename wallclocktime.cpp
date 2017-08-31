
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

#include "wallclocktime.h"

double wallclocktime(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);

    return (double)tv.tv_sec + ((double)tv.tv_usec / 1000000);
}

