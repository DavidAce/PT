//
// Created by david on 2016-07-24.
//

#ifndef CLASS_PT_WORKER_H
#define CLASS_PT_WORKER_H
#include <mpi.h>
#include <random>
#include <fstream>
#include <thread>
#include <chrono>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <set>
#include <iterator>
#include <models/class_model.h>
#include <class_tic_toc.h>
#include <class_PT_thermo.h>
#include "class_PT_groundstate.h"
#include <sim_parameters/n_sim_settings.h>
#include <nmspc_PT_counters_timers.h>
#include <nmspc_math_algorithms.h>
#include <nmspc_random_numbers.h>

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
public:
    class_worker(int id, int size);                 //Constructor
    //Main data structures of the PT algorithm. Needed very often.

    //MPI Communicator
    int world_ID;                   //Thread number
    int world_size;                 //Total number of threads
    //Temperature communication
    int     T_ID;
    double  T;
    ArrayXd T_ladder;
    int world_ID_up, world_ID_dn;
    bool sampling;

    //class_model, including lattice and observables
//    std::unique_ptr<class_model_base> model;
    class_model model;

    //Ground state
    class_PT_groundstate groundstate;

    //Thermodynamics
    class_thermo thermo;

    //PT Energy and Order parameter
    vector<int >   T_history;
    vector<double> E_history;
    vector<double> M_history;

    //Katzgrabber
    vector <int> n_history;
    vector <int> t_history;

    int direction;  // 1 for up, -1 for down, 0 if not applicable


    //PT acceptance criterion
    bool accept;

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
    void sweep() __attribute__((hot));
    friend std::ostream &operator<<(std::ostream &, const class_worker &);
};


#endif //
