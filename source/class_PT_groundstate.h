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
    int              maximum_storage = 10;


public:
    class_PT_groundstate(){};
    double           E_GS;
    vector<ArrayXXi> lattices_GS;
    void check(double E, ArrayXXi &lattice);
    void sync();

};


#endif //PT_CLASS_PT_MINIMUM_H
