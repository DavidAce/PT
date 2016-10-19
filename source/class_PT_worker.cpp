//
// Created by david on 2016-07-24.
//
#include "class_PT_worker.h"

#define profiling_total                	1
#define profiling_print                	1
#define profiling_sweep                	1
#define profiling_swap                 	0
#define profiling_make_MC_trial 		0
#define profiling_acceptance_criterion 	0


#define debug_accept_trial              1

using namespace std;
using namespace Eigen;
int counter::MCS;
int counter::accepts;
int counter::samples;
int counter::swap_trials;
int counter::swap_accepts;
int counter::store;
int timer::samp;
int timer::save;
int timer::cout;
int timer::swap;

//Constructors
class_worker::class_worker(int & id, int & size):
                                world_ID(id),
                                world_size(size),
                                model(PT_constants::L, PT_constants::J),
                                t_total                (profiling_total,                3,"Total Time"),
                                t_print                (profiling_print,                3,"Time"),
                                t_sweep                (profiling_sweep,                3,"t_sweep"),
                                t_swap                 (profiling_swap,                 3,"t_swap" ),
                                t_make_MC_trial        (profiling_make_MC_trial,        3,"t_mkMC") ,
                                t_acceptance_criterion (profiling_acceptance_criterion, 3,"t_accr")
{

    rn::rng.seed((unsigned long)world_ID);
    model.randomize_lattice();
    E        = model.get_E();
    M        = model.get_M();
    E_avg    = E;
    M_avg    = M;
    E_avg_sq = E*E;
    M_avg_sq = M*M;

    start_counters();
    set_initial_temperatures();
    cout << "ID: " << world_ID << " Started OK"<<endl;
}



void class_worker::start_counters() {
    counter::MCS                = 0;
    counter::accepts            = 0;
    counter::samples            = 0;
    counter::swap_trials        = 0;
    counter::swap_accepts       = 0;
    counter::store              = 0;
    timer::cout                = 0;
    timer::swap 				= 0;
}


void class_worker::set_initial_temperatures(){
    T_ladder  = ArrayXd::LinSpaced(world_size, PT_constants::T_min, PT_constants::T_max);
    T = T_ladder(world_ID);
    T_ID = world_ID;
    world_ID_up = math::mod(world_ID + 1, world_size);
    world_ID_dn = math::mod(world_ID - 1, world_size);
}

void class_worker::sweep(){
    t_sweep.tic();
    for (int i = 0; i < PT_constants::N ; i++){
        model.make_new_state(E,M, E_trial, M_trial);
        if(rn::uniform_double_1() < fmin(1,exp(-(E_trial - E)/T))){
            E                           = E_trial;
            M                           = M_trial;
            model.flip();
        }
    }
    t_sweep.toc();
}


//Function for printing the lattice. Very easy if L is an Eigen matrix.
std::ostream &operator<<(std::ostream &os, const class_worker &worker) {
    os << setprecision(2);
    os << "ID: " << worker.world_ID
       << " Current State: "
       << "     E = " << worker.E
       << "     M = " << worker.M  << endl
       << "     E_trial = " << worker.E_trial
       << "     M_trial = " << worker.M_trial
       << endl;
    return os;
}



