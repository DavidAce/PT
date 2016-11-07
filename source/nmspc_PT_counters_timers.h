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
    extern int swap_trials;
    extern int swap_accepts;
    extern int store;

	
};

namespace timer {
    extern int samp;
    extern int save;
    extern int comp;
    extern int cout;
    extern int swap;

}


#endif //WL_COUNTERS_TIMERS_H
