//
// Created by david on 9/26/16.
//

#ifndef PT_PT_H
#define PT_PT_H
#include <iomanip>
#include <thread>
#include <chrono>
#include <ratio>
#include <numeric>
#include "class_PT_worker.h"
#include "class_PT_output.h"
#include "nmspc_PT_constants.h"
#include "nmspc_PT_parallel_algorithms.h"
void paralleltempering  (class_worker &worker);
void warmup             (class_worker &worker, output &out);
void sample             (class_worker &worker, output &out);
void probe              (class_worker &worker);
void print_status       (class_worker &worker, bool override);

template <typename T>
void debug_print        (class_worker &worker, T input){
    MPI_Barrier(MPI_COMM_WORLD);
    if (worker.world_ID == 0) {
        cout << input;
        cout.flush();
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    MPI_Barrier(MPI_COMM_WORLD);
}


#endif //PT_PT_H
