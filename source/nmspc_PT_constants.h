//
// Created by david on 2016-07-24.
//

#ifndef PT_NMSPC_PT_CONSTANTS_H
#define PT_NMSPC_PT_CONSTANTS_H

#include "class_PT_input.h"

namespace PT_constants {
    //Job name (used for input and output of files)
    extern std::string job_name;
    extern std::string model_name;
    extern std::string input_file_name;
    extern std::string output_path;


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

    //Rates for checking, sample, storing and printing (MCS units)
    static const int rate_prob = 1;       //How often to sample
    static const int rate_sort = 10000;   //How often to sort all samplings and write timeseries data to file
    static const int rate_comp = 50000;   //How often to compute statistics for printing mid-simulation
    static const int rate_swap = 40;      //How often to swap walkers in adjacent windows
    static const int rate_cout = 10000;   //How often to cout in terminal
    static const int rate_move = 5000;    //How often to move temperatures during warmup
    static const int rate_sync = 100;     //How often to sync groundstates during warmup (to find the lowest energy states)

    //Number of calc_thermo cycles for error calculation
    static const int bootstraps = 100;
}

#endif //PT_NMSPC_PT_CONSTANTS_H
