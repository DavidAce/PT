//
// Created by david on 11/15/16.
//

#include <iostream>
#include "class_PT_groundstate.h"
#include "nmspc_PT_counters_timers.h"


void class_PT_groundstate::check(double E, ArrayXXi &lattice) {
    if (lattices_GS.empty() or E < E_GS){
        E_GS = E;
        energies_GS.clear();
        lattices_GS.clear();
        energies_GS.push_back(E_GS);
        lattices_GS.push_back(lattice);
    }else if (E == E_GS){
        //Check if the lattice has already been stored
        for (int i = 0; i < (int)lattices_GS.size(); i++){
            if ((lattices_GS[i] == lattice).all()){
                return;
            }
        }
        //If you made it here, the lattice is new and should be stored
        if ((int)lattices_GS.size() <= maximum_storage){
            energies_GS.push_back(E_GS);
            lattices_GS.push_back(lattice);
        }
    }
}


void class_PT_groundstate::sync() {
    timer::sync = 0;
    if (lattices_GS.empty()){
        return;
    }
    int world_size, world_ID;
    int GS_ID;
    int GS_num = (int)lattices_GS.size();
    assert(lattices_GS.size() == energies_GS.size() and "Size mismatch on collected ground states!");

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         //Get total number of threads
    MPI_Comm_rank(MPI_COMM_WORLD, &world_ID);           //Establish thread number of this worker

    ArrayXd E_GS_world(world_size);
    ArrayXi GS_num_world(world_size);

    MPI_Allgather(&E_GS,1,MPI_DOUBLE,E_GS_world.data(),1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Allgather(&GS_num,1,MPI_INT,GS_num_world.data(),1, MPI_INT, MPI_COMM_WORLD);

    bool E_same = (E_GS_world == E_GS_world(0)).all();
    bool n_same = (GS_num_world == GS_num_world(0)).all();
    if(E_same && n_same){return;}
    if(!E_same){
        //If not the same energies, somebody has the lowest.
        E_GS   = E_GS_world.minCoeff(&GS_ID);
        GS_num = GS_num_world(GS_ID);
        //Now GS_ID has the world ID of the thread with the lowest energy lattice found yet
    }else{
        //If all have the same energy, somebody has the most number of lattices
        GS_num = GS_num_world.maxCoeff(&GS_ID);
        E_GS   = E_GS_world(GS_ID);
    }

    ArrayXXi lattice_recv = lattices_GS[0]; // Make up a receive buffer and fill it with a dummy lattice with correct size
    double   energy_recv  = energies_GS[0];
    if (world_ID != GS_ID){
        energies_GS.clear();
        lattices_GS.clear();
    }
    for(int i = 0; i < GS_num; i++){
        if (world_ID == GS_ID){
            energy_recv  = energies_GS[i];
            lattice_recv = lattices_GS[i];
        }
        MPI_Bcast(&energy_recv, 1, MPI_DOUBLE, GS_ID, MPI_COMM_WORLD);
        MPI_Bcast(lattice_recv.data(), (int)lattice_recv.size(),MPI_INT, GS_ID, MPI_COMM_WORLD);
        if (world_ID != GS_ID){
            energies_GS.push_back(energy_recv);
            lattices_GS.push_back(lattice_recv);
        }
    }
}
