//
// Created by david on 12/5/16.
//
#include <fstream>
#include "class_model_J1J2.h"

void J1J2::randomize_lattice() {
    for(int i = 0; i < L ; i++){
        for(int j= 0; j < L ; j++){
            lattice(i,j) = 2 * rn::uniform_integer_1() - 1;
        }
    }
}


void J1J2::set_E_initial() {
    E = 0;
    for(int i = 0; i < L; i ++){
        for(int j = 0; j < L; j ++) {
            E += - J1 * lattice(i,j) * sum_neighbours(i,j);
        }
    }
    E = (E / 2.0);
}


void J1J2::set_M_initial() {
    M = lattice.sum();
}


//Function for printing the lattice. Very easy if L is an Eigen matrix.
std::ostream &operator<<(std::ostream &os, const J1J2 &j1j2){
    os << j1j2.lattice << std::endl;
    return os;
}