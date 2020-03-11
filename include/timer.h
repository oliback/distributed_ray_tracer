#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
 
class Timer {
public:
    static int timer();
    static constexpr float SECOND_DIVIDER = 1000000.0f;
};
#endif
