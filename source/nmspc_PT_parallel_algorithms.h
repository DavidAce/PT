//
// Created by david on 2016-08-11.
//

#ifndef CLASS_WL_PARALLEL_ALGORITHMS_H
#define CLASS_WL_PARALLEL_ALGORITHMS_H
#include <thread>
#include <chrono>
#include "class_tic_toc.h"
#include <Eigen/Dense>
#include <Eigen/Core>


#include "class_PT_worker.h"
#include "class_PT_output.h"
//class class_worker;
namespace mpi {
    extern void swap            (class_worker &) ;
    extern void store           (class_worker &, output &, bool force) ;

    template <typename Derived, typename mpitype>
    void bcast_dynamic (ArrayBase<Derived> &arr, mpitype MPI_TYPE , int master_id){
        int rows = (int) arr.rows();
        int cols = (int) arr.cols();
        MPI_Bcast(&rows, 1, MPI_INT, master_id, MPI_COMM_WORLD);
        MPI_Bcast(&cols, 1, MPI_INT, master_id, MPI_COMM_WORLD);
        arr.derived().resize(rows, cols);
        MPI_Bcast(arr.derived().data(), cols*rows, MPI_TYPE, master_id, MPI_COMM_WORLD);
    }
    template <typename Derived, typename mpitype>
    void send_dynamic (ArrayBase<Derived> &arr, mpitype MPI_TYPE, int dest_id){
        int rows = (int) arr.rows();
        int cols = (int) arr.cols();
        MPI_Send(&rows, 1, MPI_INT, dest_id, 0, MPI_COMM_WORLD);
        MPI_Send(&cols, 1, MPI_INT, dest_id, 1, MPI_COMM_WORLD);
        MPI_Send(arr.derived().data(), cols*rows, MPI_TYPE, dest_id,2, MPI_COMM_WORLD);
    }

    template <typename Derived, typename mpitype>
    void recv_dynamic (ArrayBase<Derived> &arr, mpitype MPI_TYPE , int src_id){
        int rows = (int) arr.rows();
        int cols = (int) arr.cols();
        MPI_Recv(&rows, 1, MPI_INT, src_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&cols, 1, MPI_INT, src_id, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        arr.derived().resize(rows, cols);
        MPI_Recv(arr.derived().data(),  cols*rows, MPI_TYPE, src_id, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    template <typename T>
    void debug_print        (class_worker &worker, T input){
        MPI_Barrier(MPI_COMM_WORLD);
        if (worker.world_ID == 0) {
            cout << input;
            cout.flush();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }


};


#endif //CLASS_WL_PARALLEL_ALGORITHMS_H
