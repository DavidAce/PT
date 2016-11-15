//
// Created by david on 11/15/16.
//

#include "class_PT_groundstate.h"


void class_PT_groundstate::check_groundstate(double &E, ArrayXXi &lattice ) {
    if (empty){
        E_groundstate = E;
        lattice_groundstate.push_back(lattice);
    }else if( E < E_groundstate ){
        E_groundstate = E;
        lattice_groundstate.clear();
        lattice_groundstate.push_back(lattice);
    }else if (E == E_groundstate){
        //Check if the lattice has already been stored
        bool exists = false;
        for (int i = 0; i < lattice_groundstate.size(); i++){
            if (lattice_groundstate[i].cwiseEqual(lattice).all()){
                exists = true;
                break;
            }
        }
        if (!exists){
            lattice_groundstate.push_back(lattice);
        }
    }
}


void class_PT_groundstate::sync_groundstates() {

    if (empty){
        return;
    }

    int world_size;
    int groundstate_ID;
    int groundstate_num = (int)lattice_groundstate.size();


    ArrayXd energies(world_size);
    ArrayXi groundstates(world_size);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         //Get total number of threads
    MPI_Allgather(&E_groundstate,1,MPI_DOUBLE,energies.data(),1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Allgather(&groundstate_num,1,MPI_INT,groundstates.data(),1, MPI_INT, MPI_COMM_WORLD);

    if (energies )

    double dummy = energies.minCoeff(&groundstate_ID);
    groundstate_num = groundstates(groundstate_ID);

    ArrayXXi lattice_recv = lattice_groundstate[0];
    for(int i = 0; i < groundstate_num; i++){
        MPI_Bcast(lattice_groundstate[i].data(), (int)lattice_groundstate[i].size(),MPI_INT, groundstate_ID, MPI_COMM_WORLD);


    }
}
