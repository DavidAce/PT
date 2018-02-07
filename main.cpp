#include <iostream>
#include <mpi.h>
#include <gitversion.h>
#include <IO/class_file_reader.h>
#include <PT.h>
#include <class_PT_worker.h>

int main(int argc, char **argv) {

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    int world_ID,world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_ID);           //Establish thread number of this worker
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         //Get total number of threads
    if(world_ID == 0) {
        std::cout << "MPI_Init started " << world_size << " threads." << std::endl;

        // Print current Git status
        std::cout << "Git Branch: " + GIT::BRANCH +
                     " | Commit hash: " + GIT::COMMIT_HASH +
                     " | Revision: " + GIT::REVISION << std::endl << std::flush;
    }
    //If input file is given as command line argument, then use that.
    std::string inputfile = "input.cfg";
    for (int i = 0; i < argc; i++){
        std::string s1 = argv[i];
        std::string s2 = ".cfg";
        if (s1.find(s2) != std::string::npos) {
            std::cout << "Given inputfile: " << s1 << std::endl;
            inputfile = s1;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    class_file_reader indata(inputfile);
    settings::initialize(indata);
    PT::paralleltempering(world_ID, world_size);
    MPI_Finalize();
    return 0;
}