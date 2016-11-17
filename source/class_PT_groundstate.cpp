//
// Created by david on 11/15/16.
//

#include "class_PT_groundstate.h"
#include "nmspc_PT_counters_timers.h"


void class_PT_groundstate::check(double &E, ArrayXXi &lattice) {
    if (lattices_GS.empty()){
        E_GS = E;
        lattices_GS.push_back(lattice);
    }else if( E < E_GS ){
        E_GS = E;
        lattices_GS.clear();
        lattices_GS.push_back(lattice);
    }else if (E == E_GS){
        //Check if the lattice has already been stored
        for (int i = 0; i < lattices_GS.size(); i++){
            if ((lattices_GS[i] == lattice).all()){
                return;
            }
        }
        //If you made it here, the lattice is new and should be stored
        if (lattices_GS.size() <= maximum_storage){
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


    MPI_Comm_size(MPI_COMM_WORLD, &world_size);         //Get total number of threads
    MPI_Comm_rank(MPI_COMM_WORLD, &world_ID);           //Establish thread number of this worker

    ArrayXd E_GS_all(world_size);
    ArrayXi num_GS_all(world_size);

    MPI_Allgather(&E_GS,1,MPI_DOUBLE,E_GS_all.data(),1, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Allgather(&GS_num,1,MPI_INT,num_GS_all.data(),1, MPI_INT, MPI_COMM_WORLD);
//    cout << E_GS_all.transpose() << endl;
//    cout << num_GS_all.transpose() << endl << endl;
    bool E_same = true, num_same = true;
    for (int i = 0; i < world_size ; i++){
        E_same = E_same && E_GS_all(i) == E_GS_all(0);
        num_same = num_same && num_GS_all(i) == num_GS_all(0);
    }
    if(E_same && num_same){return;}
    if(!E_same){
        E_GS   = E_GS_all.minCoeff(&GS_ID);
        GS_num = num_GS_all(GS_ID);
    }else{
        GS_num = num_GS_all.maxCoeff(&GS_ID);
        E_GS   = E_GS_all(GS_ID);
    }

    ArrayXXi lattice_recv = lattices_GS[0];
    if (world_ID != GS_ID){
        lattices_GS.clear();
    }
    for(int i = 0; i < GS_num; i++){
        if (world_ID == GS_ID){
            lattice_recv = lattices_GS[i];
        }
        MPI_Bcast(lattice_recv.data(), (int)lattice_recv.size(),MPI_INT, GS_ID, MPI_COMM_WORLD);
        if (world_ID != GS_ID){
            lattices_GS.push_back(lattice_recv);
        }
    }

}
