//
// Created by david on 2016-07-24.
//

#ifndef WL_COUNTERS_TIMERS_H
#define WL_COUNTERS_TIMERS_H
#include <chrono>

namespace counter {
    //Counters
    extern int MCS;
    extern int accepts;
    extern int samples;
    extern int swaps;
    extern int swap_accepts;
    extern int merges;
	
};

namespace timer {
    extern int backup;
    extern int print;
    extern int swap;
}


#endif //WL_COUNTERS_TIMERS_H
