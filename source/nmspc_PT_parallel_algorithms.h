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
#include <thread>

#include "class_PT_worker.h"
#include "class_PT_output.h"

#include "nmspc_mpi_extensions.h"
//class class_worker;
namespace parallel {
    extern void measurements(class_worker &) ;
    extern void swap_workers(class_worker &) ;
    extern void save_buffers(class_worker &, output &) ;
    extern void compute_fast(class_worker &, output &) ;
    extern void katz_Tladder(class_worker &);
    extern void compute_full(class_worker &, output &) ;

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
