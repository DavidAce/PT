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
            counter::swaps++;
            int swap;
            double E_up;
            double P_swap;      //Swap probability
            bool myTurn = math::mod(worker.T_ID, 2) == math::mod(counter::swaps, 2);

            int  up, dn;
            for (int i = 0; i < worker.world_size; i++){
                if (math::mod(worker.T_ID + 1, worker.world_size) == worker.T_address(i)){up = i;}
                if (math::mod(worker.T_ID - 1, worker.world_size) == worker.T_address(i)){dn = i;}
            }

            if (debug_swap) {
                for (int w = 0; w < worker.world_size; w++) {
                    if (w == worker.world_ID) {
                        cout << "ID: " << w << " Starting Swap. Myturn = " << myTurn << endl;
                    }
                    MPI_Barrier(MPI_COMM_WORLD);
                }
            }
            //Send current E neighbors dn and receive from above.
            MPI_Sendrecv(&worker.E, 1, MPI_DOUBLE, dn, 100, &E_up, 1, MPI_DOUBLE, up, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

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
                MPI_Send(&swap, 1, MPI_INT, up, 110, MPI_COMM_WORLD);
            } else {
                MPI_Recv(&swap, 1, MPI_INT, dn, 110, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
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
            //Now do the swapping if you got lucky
            if (swap == 1) {
                if (myTurn == 1) {
                    worker.T_ID = math::mod(worker.T_ID + 1, worker.world_size);
                    worker.T = worker.T_ladder(worker.T_ID);

                }
                else {
                    worker.T_ID = math::mod(worker.T_ID - 1, worker.world_size);
                    worker.T = worker.T_ladder(worker.T_ID);
                }
            }
            MPI_Allgather(&worker.T_ID, 1, MPI_INT, worker.T_address.data(), 1, MPI_INT, MPI_COMM_WORLD);
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

}


