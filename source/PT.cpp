//
// Created by david on 9/26/16.
//

#include "PT.h"

#define debug_sweep                     0
#define debug_status                    1



void paralleltempering(class_worker &worker){
    output out(worker.world_ID);
    worker.t_total.tic();
    worker.t_print.tic();
    for (int i = 0; i < PT_constants::MCS_sample + PT_constants::MCS_warmup; i++){
        sweep          (worker);
        sample         (worker);
        mpi::store     (worker,out,false);
        mpi::swap      (worker);
        print_status   (worker);
    }
    mpi::store(worker,out,true);
}

void sweep(class_worker &worker){
    if(debug_sweep){debug_print(worker,"Starting MCS " + std::to_string(counter::MCS));}
    worker.t_sweep.tic();
    for (int i = 0; i < PT_constants::N ; i++){
        worker.make_MC_trial();
        worker.acceptance_criterion();
        if(worker.accept){
            worker.accept_MC_trial();
        }else{
            worker.reject_MC_trial();
        }
    }
    counter::MCS++;
    worker.t_sweep.toc();
}

void sample(class_worker &worker){
    if (counter::MCS >= PT_constants::MCS_warmup) {
        worker.T_history(counter::samples) = worker.T_ID;
        worker.E_history(counter::samples) = worker.E;
        worker.M_history(counter::samples) = worker.M;
        counter::samples++;
    }
}

void print_status(class_worker &worker) {
    if (timer::print++ >= PT_constants::rate_print_status){
        timer::print = 0;
        worker.t_total.toc();
        worker.t_print.toc();
        for (int i = 0; i < worker.world_size; i++){
            if(worker.T_ID == i){
                if (worker.T_ID == 0){cout << endl;}
                cout << fixed << showpoint;
                cout << "T_ID: "        << left << setw(3) << i;
                cout << " T: "          << left << setw(6) << worker.T;

                if(debug_status){
                    cout << " E: "     << left << setw(9) << setprecision(2)   << worker.E
                         << " M: "     << left << setw(9) << setprecision(2)   << worker.M;
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
