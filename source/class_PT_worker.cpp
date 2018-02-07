//
// Created by david on 2016-07-24.
//
#include <set>
#include <iterator>
#include <mpi.h>
#include <random>
#include <sim_parameters/n_sim_settings.h>
#include <nmspc_math_algorithms.h>
#include <nmspc_random_numbers.h>
#include "class_PT_worker.h"

#define profiling_total                	1
#define profiling_print                	1
#define profiling_sweep                	0
#define profiling_swap                 	0
#define profiling_make_MC_trial 		0
#define profiling_acceptance_criterion 	0


#define debug_accept_trial              1

using namespace std;
using namespace Eigen;
int counter::MCS;
int counter::accepts;
int counter::trials;
int counter::samples;
int counter::swap_trials;
int counter::swap_accepts;
int counter::store;
int timer::samp;
int timer::save;
int timer::comp;
int timer::cout;
int timer::swap;
int timer::move;
int timer::sync;

//Constructors
class_worker::class_worker(int id, int size):
                                world_ID(id),
                                world_size(size),
                                t_total                (profiling_total,                3,"Total Time"),
                                t_print                (profiling_print,                3,"Time"),
                                t_sweep                (profiling_sweep,                3,"t_sweep"),
                                t_swap                 (profiling_swap,                 3,"t_swap" ),
                                t_make_MC_trial        (profiling_make_MC_trial,        3,"t_mkMC") ,
                                t_acceptance_criterion (profiling_acceptance_criterion, 3,"t_accr")
{
    rn::rng.seed((unsigned long)world_ID);
    sampling     = false;
    direction    = 0;
    start_counters();
    set_initial_temperatures();
    T_ID_list.resize((unsigned long) world_size);
    MPI_Allgather(&T_ID,1,MPI_INT,T_ID_list.data(),1, MPI_INT, MPI_COMM_WORLD);
    cout << "ID: " << world_ID << " Started OK"<<endl;

}



void class_worker::start_counters() {
    counter::MCS                = 0;
    counter::accepts            = 0;
    counter::trials             = 0;
    counter::samples            = 0;
    counter::swap_trials        = 0;
    counter::swap_accepts       = 0;
    counter::store              = 0;
    timer::cout                 = 0;
    timer::comp                 = 0;
    timer::swap 				= 0;
    timer::move 				= 0;
    timer::sync 				= 0;
}


void class_worker::set_initial_temperatures(){
    T_ladder  = ArrayXd::LinSpaced(world_size, settings::sim::T_min, settings::sim::T_max);
    T = T_ladder(world_ID);
    T_ID = world_ID;
    world_ID_up = math::mod(world_ID + 1, world_size);
    world_ID_dn = math::mod(world_ID - 1, world_size);
    thermo.T = T;
}

void class_worker::sweep(){
    t_sweep.tic();
    for (int i = 0; i < settings::model::N ; i++){
        model.make_new_state();
        counter::trials++;
        if(rn::uniform_double_1() < fmin(1,exp(-(model.E_trial - model.E)/T))) {
            counter::accepts++;
            model.accept_state();
        }
    }
    t_sweep.toc();
}

void class_worker::sweep(class_PT_groundstate &GS){
    t_sweep.tic();
    for (int i = 0; i < settings::model::N ; i++){
        model.make_new_state();
        counter::trials++;
        if(rn::uniform_double_1() < fmin(1,exp(-(model.E_trial - model.E)/T))) {
            counter::accepts++;
            model.accept_state();
            GS.check(model.E, model.lattice);
        }
    }
    t_sweep.toc();
}



//Function for printing the lattice. Very easy if L is an Eigen matrix.
std::ostream &operator<<(std::ostream &os, const class_worker &worker) {
    os << setprecision(2);
    os << "ID: " << worker.world_ID
       << " Current State: "
       << "     E = " << worker.model.E
       << "     M = " << worker.model.M << endl
       << "     E_trial = " << worker.model.E_trial
       << "     M_trial = " << worker.model.M_trial
       << endl;
    return os;
}



