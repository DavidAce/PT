//
// Created by david on 2016-07-24.
//

#ifndef WL_CLASS_WL_WORKER_H
#define WL_CLASS_WL_WORKER_H
#include <mpi.h>
#include <random>
#include <fstream>
#include <thread>
#include <chrono>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <set>
#include <iterator>
#include "class_PT_input.h"
#include "class_PT_output.h"
#include "class_model.h"
#include "class_tic_toc.h"
#include "nmspc_PT_constants.h"
#include "nmspc_PT_counters_timers.h"
#include "nmspc_math_algorithms.h"
#include "nmspc_random_numbers.h"

using namespace Eigen;

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    if ( !v.empty() ) {
        out << "[ ";
        std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, " "));
        out << "]";
    }
    return out;
}



class class_worker {
private:

public:
    class_worker(int &id, int &size);                 //Constructor
    //Main data structures of the WL algorithm. Needed very often.


    //MPI Communicator
    int world_ID;                   //Thread number
    int world_size;                 //Total number of threads
    //Temperature communication
    int T_ID;
    double T;
    ArrayXd T_ladder;
//    ArrayXi T_address;  //Array mapping world_ID  to which T_ID
    int world_ID_up, world_ID_dn;
    double E_avg, E_avg_sq;
    double M_avg, M_avg_sq;

    //Read simulation specifications
    input in;
    output out;
    //Lattice
    class_model model;
    //PT Energy and Order parameter
    double E,M;                         //Current Energy and Order parameter
    double E_trial, M_trial;                 //Proposed
    Array<int ,   PT_constants::rate_store_samples, 1> T_history;
    Array<double, PT_constants::rate_store_samples ,1> E_history;
    Array<double, PT_constants::rate_store_samples ,1> M_history;

    //PT acceptance criterion
    bool accept;

    int     finish_line;                 //turns to 1 when converged

	//Used for profiling functions in worker
    class_profiling t_total                 ,
                    t_print                 ,
                    t_sweep 				,
					t_swap 					,
					t_make_MC_trial 		,
					t_acceptance_criterion 	;
    //Functions
    void start_counters();
    void set_initial_temperatures();
    void make_MC_trial() __attribute__((hot));
    void acceptance_criterion() __attribute__((hot));
    void accept_MC_trial() __attribute__((hot));
    void reject_MC_trial() __attribute__((hot));
    friend std::ostream &operator<<(std::ostream &, const class_worker &);
};


#endif //WL_CLASS_WL_WORKER_H
