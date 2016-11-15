//
// Created by david on 11/15/16.
//

#ifndef PT_CLASS_PT_groundstate_H
#define PT_CLASS_PT_groundstate_H
#include <Eigen/Core>
#include <mpi.h>
#include <vector>
using namespace Eigen;
using namespace std;
class class_PT_groundstate {
private:
    bool             empty;
    double           E_groundstate;
    vector<ArrayXXi> lattice_groundstate;
public:
    class_PT_groundstate(){
        empty = true;
    };
    void check_groundstate(double &E, ArrayXXi &lattice);
    void sync_groundstates();

};


#endif //PT_CLASS_PT_MINIMUM_H
