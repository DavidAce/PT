//
// Created by david on 2016-07-24.
//

#ifndef PT_NMSPC_PT_CONSTANTS_H
#define PT_NMSPC_PT_CONSTANTS_H

#include "class_PT_input.h"

namespace PT_constants {
    //Job name (used for input and output of files)
    extern std::string job_name;
    extern std::string filename;

    //PT class_thermo properties
    extern double T_min;            //Minimum temperature for thermodynamic quantities
    extern double T_max;            //Maximum temperature for thermodynamic quantities

    //Lattice Properties
    extern int J;                   //Interaction strength (coupling)
    extern int d;                   //Dimension
    extern int L;                   //Linear size
    extern int N;                   //Number of spins/particles

    //PT simulation length
    extern int  MCS_warmup;
    extern int  MCS_sample;
    extern void copy_input(input &in);

    //Rates for checking, sampling, storing and printing (MCS units)
    static const int rate_samp = 1;
    static const int rate_save = 10000;
    static const int rate_comp = 100000;     //How often to compute statistics
    static const int rate_swap = 50;       //How often to swap walkers in adjacent windows
    static const int rate_cout = 10000;     //How often to cout in terminal

    //Number of compute cycles for error calculation
    static const int bootstraps = 100;
}

#endif //PT_NMSPC_PT_CONSTANTS_H
