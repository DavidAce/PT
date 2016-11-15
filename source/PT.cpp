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

    warmup(worker);
    sample(worker,out);
    parallel::sort(worker, out, true);
    parallel::save(worker, out);
    print_status(worker, true);
}


void warmup(class_worker &worker){
    while (counter::MCS < PT_constants::MCS_warmup) {
        worker.sweep();
        if (timer::swap >= PT_constants::rate_swap) { parallel::swap(worker); }
        if (timer::move >= PT_constants::rate_move) { parallel::katz(worker); }
        if (timer::cout >= PT_constants::rate_cout) { print_status(worker,false); }
        counter::MCS++;
        timer::swap++;
        timer::cout++;
        timer::move++;
    }
    //Make some more moves to be sure, in case T-movement happened recently
    for (int i = 0; i < 1000; i++){
        worker.sweep();
    }
    counter::MCS = 0;
}

void sample(class_worker &worker, output &out){
    worker.sampling = true;
    while (counter::MCS < PT_constants::MCS_sample) {
        worker.sweep();
        if (timer::prob >= PT_constants::rate_prob) { probe(worker); }
        if (timer::sort >= PT_constants::rate_sort) { parallel::sort(worker, out, false); }
        if (timer::swap >= PT_constants::rate_swap) { parallel::swap(worker); }
        if (timer::cout >= PT_constants::rate_cout) { print_status(worker,false); }
        counter::MCS++;
        timer::prob++;
        timer::sort++;
        timer::calc++;
        timer::swap++;
        timer::cout++;
    }
}



void probe(class_worker &worker){
    timer::prob = 0;
    worker.T_history.push_back(worker.T_ID);
    worker.E_history.push_back(worker.E);
    worker.M_history.push_back(worker.M);

}

void print_status(class_worker &worker,bool override) {
    if (timer::cout >= PT_constants::rate_cout || override){
        timer::cout = 0;
        worker.t_total.toc();
        worker.t_print.toc();
        if (timer::calc >= PT_constants::rate_comp) { parallel::calc(worker, false); }else{worker.thermo.reset();}
        for (int i = 0; i < worker.world_size; i++){
            if(worker.T_ID == i){
                if (worker.T_ID == 0){cout << endl;}
                cout << fixed << showpoint;
                cout << "T_ID: "        << left << setw(3) << i;

                if(print_extra) {
                    cout << "W_ID: " << left << setw(3) << worker.world_ID;
                    cout << " T: " << left << setw(6)  << setprecision(3) << worker.T;
                    cout << " A: " << left << setw(6)  << setprecision(3) << counter::accepts/(double)counter::trials;
                    cout << " SA: " << left << setw(6)  << setprecision(3) << counter::swap_accepts/(double)counter::swap_trials;
                    cout << " u: " << left << setw(6) << setprecision(4)  << worker.thermo.u
                            << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u_flyv << ")"
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u_tau << ")"
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u_tau2 << ")"
                            << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_u << ")";
                    cout << " m: " << left << setw(6) << setprecision(4)  << worker.thermo.m
                            << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_m     << ")";
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_m_tau << ")"
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_m_tau2 << ")";
                    cout << " c: " << left << setw(6) << setprecision(4)  << worker.thermo.c
                            << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_c << ")";
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_c_tau << ")"
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_c_tau2 << ")";
                    cout << " x: " << left << setw(6) << setprecision(4)  << worker.thermo.x
                            << "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_x << ")";
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_x_tau << ")"
                            //<< "(" << left << setw(6) << setprecision(7)  << worker.thermo.sigma_x_tau2 << ")";
                    cout << " tau: " << left << setw(8) << setprecision(4) << worker.thermo.tau_E ;
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
