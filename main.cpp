#include <iostream>
#include <mpi.h>
#include "source/PT.h"
#include "source/class_PT_input.h"
int main(int argc, char **argv) {


    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_ID,world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_ID);           //Establish thread number of this worker
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         //Get total number of threads
    std::vector<std::string> args(argv, argv + argc);
    std::string settings_file;
    if (argc < 2){
        if (world_ID == 0){std::cout << "Running with default settings" << endl;}
        settings_file = "input/default.dat";
    }else{
        if (world_ID == 0){std::cout << "Running with file: " << args[1] << endl;}
        settings_file = args[1];
    }
    input in(settings_file, world_ID);
    PT_constants::copy_input(in);


    class_worker worker(world_ID, world_size);
    paralleltempering(worker);
    MPI_Finalize();

    return 0;
}