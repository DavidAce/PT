#include <iostream>
#include <mpi.h>
#include <gitversion.h>
#include <IO/class_file_reader.h>
#include <PT.h>

int main(int argc, char **argv) {

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_ID,world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_ID);           //Establish thread number of this worker
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         //Get total number of threads

    if(world_ID == 0) {
        // Print current Git status
        std::cout << "Git Branch: " + GIT::BRANCH +
                     " | Commit hash: " + GIT::COMMIT_HASH +
                     " | Revision: " + GIT::REVISION << std::endl << std::flush;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "ID " << world_ID << ":  reading argc" << endl << flush;
    //If input file is given as command line argument, then use that.
    std::string inputfile = argc > 1 ? std::string(argv[0]) : std::string("input.cfg");
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "ID " << world_ID << ":  reading file" << endl << flush;
    class_file_reader indata(inputfile);
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "ID " << world_ID << ":  initializing indata" << endl<< flush;
    settings::initialize(indata);
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "ID " << world_ID << ":  initializing worker" << endl<< flush;
    class_worker worker(world_ID, world_size);
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "ID " << world_ID << ":  running paralleltempering" << endl<< flush;

    PT::paralleltempering(worker);
    MPI_Finalize();
    return 0;
}