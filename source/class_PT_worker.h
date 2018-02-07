//
// Created by david on 2016-07-24.
//

#ifndef CLASS_PT_WORKER_H
#define CLASS_PT_WORKER_H

#include <Eigen/Core>
#include <models/class_model.h>
#include <class_tic_toc.h>
#include <class_PT_thermo.h>
#include <class_PT_groundstate.h>
#include <nmspc_PT_counters_timers.h>


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
    Eigen::ArrayXd T_ladder;
    vector<int> T_ID_list;
    int world_ID_up, world_ID_dn;
    bool sampling;

    //class_model, including lattice and observables
    class_model model;

    //Thermodynamics
    class_thermo thermo;

    //PT Energy and Order parameter
//    vector<int >   T_history;
    vector<double> E_history;
    vector<double> M_history;

    //Katzgrabber
    vector <int> n_history;
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
    void sweep(class_PT_groundstate &GS);
    friend std::ostream &operator<<(std::ostream &, const class_worker &);
};


#endif //
