//
// Created by david on 2016-07-24.
//

#ifndef PT_NMSPC_PT_CONSTANTS_H
#define PT_NMSPC_PT_CONSTANTS_H

#include "class_PT_input.h"

namespace PT_constants {
    //Job name (used for input and output of files)
    extern std::string job_name;

    //PT thermodynamics properties
    extern double T_min;            //Minimum temperature for thermodynamic quantities
    extern double T_max;            //Maximum temperature for thermodynamic quantities

    //Lattice Properties
    extern int J;                   //Interaction strength (coupling)
    extern int d;                   //Dimension
    extern int L;                   //Linear size
    extern int N;                   //Number of spins/particles
    //PT simulation length
    extern int MCS_warmup;
    extern int MCS_sample;
    extern void copy_input(input &in);

    //Rates for checking and printing (MCS units)
    static const int rate_swap          = 10;       //How often to swap walkers in adjacent windows
    static const int rate_print_status  = 500;     //How often to print in terminal
    static const int rate_store_samples = 10000;

}

#endif //PT_NMSPC_PT_CONSTANTS_H
