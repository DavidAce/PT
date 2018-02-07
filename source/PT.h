//
// Created by david on 9/26/16.
//

#ifndef PT_PT_H
#define PT_PT_H


class class_worker;
class output;

namespace PT {
    extern void paralleltempering(int world_ID, int world_size);
    extern void warmup(class_worker &worker, output &out);
    extern void sample(class_worker &worker, output &out);
    extern void print_status(class_worker &worker, output &out);

//    template<typename T>
//    void debug_print(class_worker &worker, T input) {
//        MPI_Barrier(MPI_COMM_WORLD);
//        if (worker.world_ID == 0) {
//            cout << input;
//            cout.flush();
//            std::this_thread::sleep_for(std::chrono::microseconds(10));
//        }
//        MPI_Barrier(MPI_COMM_WORLD);
//    }
}

#endif //PT_PT_H
