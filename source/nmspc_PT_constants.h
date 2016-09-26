//
// Created by david on 2016-07-24.
//

#ifndef PT_NMSPC_PT_CONSTANTS_H
#define PT_NMSPC_PT_CONSTANTS_H

namespace PT_constants{

    //PT boostrap properties
    static const int simulation_reps = 8;      //Number of independent do_simulations
    static const int bootstrap_reps  = 8;

    //PT simulation length
    static const int MCS_warmup = 1000;
    static const int MCS_sample = 10000;

    //PT thermodynamics properties
    static const double T_min = 0.01;               //Minimum temperature for thermodynamic quantities
    static const double T_max = 6;               //Maximum temperature for thermodynamic quantities

    //Lattice Properties
    static const int d = 2;               //Dimension
    static const int L = 8;               //Linear size
    static const int N = (int) pow(L,d);  //Number of spins/particles


    //Rates for checking and printing (MCS units)
    static const int    rate_swap              = 10;       //How often to swap walkers in adjacent windows
    static const int    rate_print_status      = 1000;     //How often to print in terminal

}


#endif //PT_NMSPC_PT_CONSTANTS_H
