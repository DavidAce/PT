//
// Created by david on 9/26/16.
//

#include "PT.h"

#define debug_sweep                     0
#define debug_status                    1
#define print_extra                     1



void paralleltempering(class_worker &worker) {
    output out(worker.world_ID);
    worker.t_total.tic();
    worker.t_print.tic();
    for (int i = 0; i < PT_constants::MCS_sample + PT_constants::MCS_warmup; i++) {
        worker.sweep();
        if (timer::samp >= PT_constants::rate_samp) { sample(worker); }
        if (timer::save >= PT_constants::rate_save) { parallel::save(worker, out, false); }
        if (timer::swap >= PT_constants::rate_swap) { parallel::swap(worker); }
        if (timer::cout >= PT_constants::rate_cout) { print_status(worker,false); }
        counter::MCS++;
        timer::samp++;
        timer::save++;
        timer::swap++;
        timer::cout++;
    }
    parallel::save(worker, out, true);
    print_status(worker, true);
}

void sample(class_worker &worker){
    if (counter::MCS >= PT_constants::MCS_warmup) {
        worker.T_history(counter::samples) = worker.T_ID;
        worker.E_history(counter::samples) = worker.E;
        worker.M_history(counter::samples) = worker.M;
        counter::samples++;
    }
}

void print_status(class_worker &worker,bool override) {
    if (timer::cout >= PT_constants::rate_cout || override){
        timer::cout = 0;
        if(print_extra && counter::store > 0){
            worker.thermo.load_data(worker.T_ID, worker.T);
            worker.thermo.compute();
        }
        worker.t_total.toc();
        worker.t_print.toc();
        for (int i = 0; i < worker.world_size; i++){
            if(worker.T_ID == i){
                if (worker.T_ID == 0){cout << endl;}
                cout << fixed << showpoint;
                cout << "T_ID: "        << left << setw(3) << i;

                if(print_extra) {
                    cout << "W_ID: " << left << setw(3) << worker.world_ID;
                    cout << " T: "   << left << setw(6)  << setprecision(3) << worker.T;
                    cout << " u: "   << left << setw(6) << setprecision(4) << worker.thermo.u << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u << ")"
                                                                                              << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u_tau << ")"
                                                                                              << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u_flyv << ")";
                    cout << " m: "   << left << setw(6) << setprecision(4) << worker.thermo.m << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_m     << ")"
                                                                                              << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_m_tau << ")";
                    cout << " c: "   << left << setw(6) << setprecision(4) << worker.thermo.c << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_c << ")"
                                                                                              << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_c_tau << ")";
                    cout << " x: "   << left << setw(6) << setprecision(4) << worker.thermo.x << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_x << ")"
                                                                                              << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_x_tau << ")";
                    cout << " tau: " << left << setw(8) << setprecision(4) << worker.thermo.tau_E << " " << left << setw(9) << worker.thermo.tau_M;
                }
                if(debug_status){
                    cout << " E: "        << left << setw(9) << setprecision(2)   << worker.E
                         << " M: "        << left << setw(9) << setprecision(2)   << worker.M;
                    cout << " E_tr: "     << left << setw(9) << setprecision(2)   << worker.E_trial
                         << " M_tr: "     << left << setw(9) << setprecision(2)   << worker.M_trial;
                }
                cout << " Sw: "    << left << setw(5) << counter::swap_accepts
                     << " MCS: "   << left << setw(10) << counter::MCS;
                worker.t_print               .print_delta();
                worker.t_sweep               .print_total_reset();
                worker.t_make_MC_trial       .print_total_reset();
                worker.t_acceptance_criterion.print_total_reset();
                worker.t_swap                .print_total_reset();
                cout << endl;
            }
            cout.flush();
            std::this_thread::sleep_for(std::chrono::microseconds(10));
            MPI_Barrier(MPI_COMM_WORLD);
        }
        cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
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
}
