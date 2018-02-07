//
// Created by david on 9/26/16.
//
#include <iomanip>
#include <thread>
#include <chrono>
#include <ratio>
#include <numeric>
#include <class_PT_worker.h>
#include <class_PT_output.h>
#include <nmspc_PT_parallel_algorithms.h>
#include <sim_parameters/n_sim_settings.h>
#include "PT.h"

#define debug_sweep                     0
#define debug_status                    0
#define print_thermo                     1


void PT::paralleltempering(int world_ID, int world_size) {
    class_worker worker(world_ID, world_size);
    output out;
    worker.t_total.tic();
    worker.t_print.tic();
    PT::warmup(worker,out);
    PT::sample(worker,out);
    // Store model and simulation specific details
    out.store_model();
    out.store_generic(worker.T_ladder,"temperatures", "T", false);
}


void PT::warmup(class_worker &worker, output &out){
    class_PT_groundstate GS;
    while (counter::MCS < settings::sim::MCS_warmup) {
        worker.sweep(GS);
        if (timer::swap >= settings::rate::swap) { timer::swap = 0; parallel::swap_workers(worker); }
        if (timer::move >= settings::rate::move) { timer::move = 0; parallel::katz_Tladder(worker); }
        if (timer::sync >= settings::rate::sync) { timer::sync = 0; GS.sync(); }
        if (timer::cout >= settings::rate::cout) { timer::cout = 0; PT::print_status(worker,out); }
        counter::MCS++;
        timer::swap++;
        timer::cout++;
        timer::move++;
        timer::sync++;
    }
    //Store the lattice groundstates into a file
    GS.sync();
    out.store_groundstates(GS.lattices_GS, GS.energies_GS);
    counter::MCS = 0;
}

void PT::sample(class_worker &worker, output &out){
    worker.sampling = true;
    while (counter::MCS < settings::sim::MCS_sample) {
        worker.sweep();
        if (timer::samp >= settings::rate::samp) { timer::samp = 0;  parallel::measurements(worker); }
        if (timer::save >= settings::rate::save) { timer::save = 0;  parallel::save_buffers(worker, out); }
        if (timer::swap >= settings::rate::swap) { timer::swap = 0;  parallel::swap_workers(worker); }
        if (timer::comp >= settings::rate::comp) { timer::comp = 0;  parallel::compute_full(worker, out); }
        if (timer::cout >= settings::rate::cout) { timer::cout = 0;  PT::print_status(worker, out); }
        counter::MCS++;
        timer::samp++;
        timer::save++;
        timer::comp++;
        timer::swap++;
        timer::cout++;
    }
    parallel::save_buffers(worker, out);
    parallel::compute_full(worker, out);
    PT::print_status(worker, out);
}



void PT::print_status(class_worker &worker, output &out) {
    worker.t_total.toc();
    worker.t_print.toc();

    for (int i = 0; i < worker.world_size; i++){
        if(worker.world_ID == i){
            if (worker.world_ID == 0){cout << endl;}
            cout << fixed << showpoint;
            cout << "W_ID: " << left << setw(3) << worker.world_ID;
            cout << "T_ID: "        << left << setw(3) << worker.T_ID;
            cout << " T: " << left << setw(6)  << setprecision(3) << worker.thermo.T;
            if(print_thermo) {
                parallel::compute_fast(worker, out);
                cout << " u: " << left << setw(6) << setprecision(4)  << worker.thermo.u
                        << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u_flyv << ")"
                        << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u << ")";
                cout << " m: " << left << setw(6) << setprecision(4)  << worker.thermo.m
                        << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_m     << ")";
                cout << " c: " << left << setw(6) << setprecision(4)  << worker.thermo.c
                        << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_c << ")";
                cout << " x: " << left << setw(6) << setprecision(4)  << worker.thermo.x
                        << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_x << ")";
                cout << " tau: " << left << setw(8) << setprecision(4)<< worker.thermo.tau_E ;
            }
            if(debug_status){
                cout << " E: "        << left << setw(9) << setprecision(2)   << worker.model.E
                     << " M: "        << left << setw(9) << setprecision(2)   << worker.model.M;
                cout << " E_tr: "     << left << setw(9) << setprecision(2)   << worker.model.E_trial
                     << " M_tr: "     << left << setw(9) << setprecision(2)   << worker.model.M_trial;
            }
            cout << " A: " << left << setw(6)  << setprecision(3) << counter::accepts/(double)counter::trials;
            cout << " SA: " << left << setw(6)  << setprecision(3) << counter::swap_accepts/(double)counter::swap_trials;
            cout << " Sw: "    << left << setw(5) << counter::swap_accepts
                 << " MCS: "   << left << setw(10) << counter::MCS;


            //See top of file class_PT_worker.cpp to activate the following timers
            worker.t_print               .print_delta();
            worker.t_sweep               .print_total_reset();
            worker.t_make_MC_trial       .print_total_reset();
            worker.t_acceptance_criterion.print_total_reset();
            worker.t_swap                .print_total_reset();
            cout << endl;
        }
        cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(5));
        MPI_Barrier(MPI_COMM_WORLD);
    }
    cout.flush();
    std::this_thread::sleep_for(std::chrono::microseconds(5));
    MPI_Barrier(MPI_COMM_WORLD);
    if (worker.world_ID == 0){
        cout    << "-----";
        worker.t_total.print_total<double>(); cout << " s";
        cout << "  -----"
             << endl;
    }
    worker.t_total.tic();
    worker.t_print.tic();

}
