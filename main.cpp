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
    std::string file_name;
    std::string model_name;
    if (argc == 1){
        if (world_ID == 0){std::cout << "Running with default settings1" << endl;}
    }else if(argc == 2){
        if (world_ID == 0){std::cout << "Running with settings from file: " << args[1] << endl;}
        file_name = args[1];
    }
    else if(argc == 3){
        if (world_ID == 0){std::cout << "Running model: " << args[1] << " with settings from file: " << args[2] << endl;}
        model_name = args[1];
        file_name = args[2];
    }else{
        if (world_ID == 0){std::cout
                    << "Wrong syntax for excecution.\n" << endl
                    << "Try" << endl
                    << "    mpirun -n [nprocs] ./[output_path]/PT" << endl
                    << "for default settings, or" << endl
                    << "    mpirun -n [nprocs] ./[output_path]/PT [input_file_name]"
                    << "or" << endl
                    << "    mpirun -n [nprocs] ./[output_path]/PT [modelname] [input_file_name]"
                    << endl;}
    }
    input in(model_name, file_name, world_ID);
    PT_constants::copy_input(in);


    class_worker worker(world_ID, world_size);
    paralleltempering(worker);
    MPI_Finalize();

    return 0;
}