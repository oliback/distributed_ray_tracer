#include "timer.h"

int Timer::timer()
{
    struct timeval tv;
    gettimeofday(&tv, (struct timezone*)0);
    return (tv.tv_sec*1000000+tv.tv_usec);
}
