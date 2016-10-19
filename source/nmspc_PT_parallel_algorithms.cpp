//
// Created by david on 2016-08-11.
//

#include "nmspc_PT_parallel_algorithms.h"
#define debug_swap      0

using namespace std;

namespace mpi {
    void swap(class_worker &worker) {
        //Use MPI Tag in the 100-200 range
        if (timer::swap++ > PT_constants::rate_swap) {
            timer::swap = 0;
            worker.t_swap.tic();
            counter::swap_trials++;
            int swap;
            double E_up;
            double P_swap;      //Swap probability
            bool myTurn = math::mod(worker.T_ID, 2) == math::mod(counter::swap_trials, 2);

//            int  up, dn;
//            for (int i = 0; i < worker.world_size; i++){
//                if (math::mod(worker.T_ID + 1, worker.world_size) == worker.T_address(i)){up = i;}
//                if (math::mod(worker.T_ID - 1, worker.world_size) == worker.T_address(i)){dn = i;}
//            }

            if (debug_swap) {
                for (int w = 0; w < worker.world_size; w++) {
                    if (w == worker.world_ID) {
                        cout << "ID: " << w << " Starting Swap. Myturn = " << myTurn << endl;
                    }
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
            //Send current E neighbors dn and receive from above.
            MPI_Sendrecv(&worker.E, 1, MPI_DOUBLE, worker.world_ID_dn, 100, &E_up, 1, MPI_DOUBLE, worker.world_ID_up, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            //Now both swappees need to know if it is ok to go ahead with a swap.
            if (debug_swap) {
                for (int w = 0; w < worker.world_size; w++) {
                    if (w == worker.world_ID) {
                        cout << "ID: " << w << " SendRecv Successful"
                             << " E_up = " << E_up
                             << endl;
                    }
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }

            if (myTurn) {
                double T_up = worker.T_ladder(math::mod(worker.T_ID + 1, worker.world_size));
                P_swap = exp((worker.E - E_up)*(1 / worker.T - 1 / T_up)); //Swap probability
                swap = rn::uniform_double_1() < fmin(1, P_swap) ? 1 : 0;
                if (worker.T_ID == worker.world_size -1 ){ swap = 0;}
                MPI_Send(&swap, 1, MPI_INT, worker.world_ID_up, 110, MPI_COMM_WORLD);
            } else {
                MPI_Recv(&swap, 1, MPI_INT, worker.world_ID_dn, 110, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            if (debug_swap) {
                for (int w = 0; w < worker.world_size; w++) {
                    if (w == worker.world_ID) {
                        cout << "ID: " << w << " Swap = " << swap  << endl;
                    }
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
            if(debug_swap){debug_print(worker,"Starting swap\n");}

            //Communicate the new world_ID before swapping

            int world_ID_dn_new = worker.world_ID_dn, world_ID_up_new = worker.world_ID_up;
            if(myTurn){
                //Receive news from world_ID_dn about who will be your new world_ID_dn
                MPI_Recv(&world_ID_dn_new, 1, MPI_INT, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //(*)I know that if I'm swapping, the variable above is not actually correct yet.
                //world_ID_dn_new will actually by my current up_new

            }else {
                //Send news up to world_ID_up that world_ID_dn will be at this position
                if (swap) {
                    MPI_Send(&worker.world_ID_dn, 1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
                } else {
                    MPI_Send(&worker.world_ID,    1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
                }
            }

            if(myTurn) {
                if (swap) {
                    //Send up
                    MPI_Send(&world_ID_dn_new, 1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
                    //Now I can correct (*)
                    world_ID_dn_new = worker.world_ID_up;
                } else {
                    MPI_Send(&worker.world_ID, 1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
                }
            }else{
                    MPI_Recv(&world_ID_dn_new, 1, MPI_INT, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            }

            if(myTurn) {
                //Send news dn to world_ID_dn who will be his new neighbor up
                if(swap){
                    MPI_Send(&worker.world_ID_up, 1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
                }else{
                    MPI_Send(&worker.world_ID   , 1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
                }
            }else{
                MPI_Recv(&world_ID_up_new, 1, MPI_INT, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //(**)I know that if I'm swapping the variable above is not actually correct yet.
                //world_ID_up_new will actually be worker.world_ID_dn
            }

            if(myTurn){
                //Receive news from world_ID_up about who will be your new world_ID_up
                MPI_Recv(&world_ID_up_new, 1, MPI_INT, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }else {
                //Send news up to world_ID_dn who will be at this position
                if (swap) {
                    MPI_Send(&world_ID_up_new, 1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
                    //Now I can correct (**)
                    world_ID_up_new = worker.world_ID_dn;
                } else {
                    MPI_Send(&worker.world_ID,    1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
                }

            }


            //Now do the swapping if you got lucky
            if (swap == 1) {
                if (myTurn == 1) {
                    worker.T_ID = math::mod(worker.T_ID + 1, worker.world_size);
                    worker.T = worker.T_ladder(worker.T_ID);
                    MPI_Sendrecv_replace(&worker.E_avg,   1,MPI_DOUBLE, worker.world_ID_up, worker.world_ID, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Sendrecv_replace(&worker.M_avg,   1,MPI_DOUBLE, worker.world_ID_up, worker.world_ID, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Sendrecv_replace(&worker.E_avg_sq,1,MPI_DOUBLE, worker.world_ID_up, worker.world_ID, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Sendrecv_replace(&worker.M_avg_sq,1,MPI_DOUBLE, worker.world_ID_up, worker.world_ID, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
               }
                else {
                    worker.T_ID = math::mod(worker.T_ID - 1, worker.world_size);
                    worker.T = worker.T_ladder(worker.T_ID);
                    MPI_Sendrecv_replace(&worker.E_avg,   1,MPI_DOUBLE, worker.world_ID_dn, worker.world_ID, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Sendrecv_replace(&worker.M_avg,   1,MPI_DOUBLE, worker.world_ID_dn, worker.world_ID, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Sendrecv_replace(&worker.E_avg_sq,1,MPI_DOUBLE, worker.world_ID_dn, worker.world_ID, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Sendrecv_replace(&worker.M_avg_sq,1,MPI_DOUBLE, worker.world_ID_dn, worker.world_ID, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }

            worker.world_ID_dn = world_ID_dn_new;
            worker.world_ID_up = world_ID_up_new;
//            MPI_Allgather(&worker.T_ID, 1, MPI_INT, worker.T_address.data(), 1, MPI_INT, MPI_COMM_WORLD);
            counter::swap_accepts += swap;
            if (debug_swap) {
                for (int w = 0; w < worker.world_size; w++) {
                    if (w == worker.world_ID) {
                        cout << "ID: " << w << " Swap OK " << endl;
                    }
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
            worker.t_swap.toc();
        }
    }

    void store(class_worker &worker, output &out, bool force){
        if (counter::samples == PT_constants::rate_store_samples || force){
            //Reorder E
            for (int i = 0; i < counter::samples; i++){
                MPI_Sendrecv_replace(&worker.E_history(i), 1, MPI_DOUBLE, worker.T_history(i),i, MPI_ANY_SOURCE,i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            //Reorder M
            for (int i = 0; i < counter::samples; i++){
                MPI_Sendrecv_replace(&worker.M_history(i), 1, MPI_DOUBLE, worker.T_history(i),i, MPI_ANY_SOURCE,i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            out.store_samples(worker.E_history, "E" +  std::to_string(worker.world_ID) + ".dat",counter::store);
            out.store_samples(worker.M_history, "M" +  std::to_string(worker.world_ID) + ".dat",counter::store);
            counter::store++;
            counter::samples = 0;
        }
    }
}


