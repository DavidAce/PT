//
// Created by david on 2016-07-24.
//
#include <fstream>
#include "class_model.h"

//using namespace std;

class_model::class_model(){
    J1  = settings::model::J1;
    on  = settings::model::on;
    L     = settings::model::L;
    N     = L*L;
    lattice.resize(L,L);
    randI = rn::uniform_integer(0,L-1);
    randJ = rn::uniform_integer(0,L-1);
    randomize_lattice();
    set_E_initial();
    set_M_initial();
};

void class_model::randomize_lattice() {
    for(int i = 0; i < L ; i++){
        for(int j= 0; j < L ; j++){
            lattice(i,j) = 2 * rn::uniform_integer_1() - 1;
        }
    }
}

void class_model::flip()  __attribute__((always_inline)) {
    lattice(randI,randJ) *= -1;
}

int class_model::sum_neighbours(const int i, const int j){
    return            lattice(math::mod(i+1,L), math::mod(j  ,L))
                      +lattice(math::mod(i  ,L), math::mod(j+1,L))
                      +lattice(math::mod(i-1,L), math::mod(j  ,L))
                      +lattice(math::mod(i  ,L), math::mod(j-1,L));
}

void class_model::make_new_state(){
    randI = rn::uniform_integer(0, L-1);
    randJ = rn::uniform_integer(0, L-1);
    E_trial = E + J1 * 2 *  lattice(randI, randJ) * sum_neighbours(randI, randJ);
    M_trial = M - 2 * lattice(randI, randJ);
};



void class_model::accept_state(){
    E = E_trial;
    M = M_trial;
    flip();
}


void class_model::set_E_initial()  {
    E = 0;
    for(int i = 0; i < L; i ++){
        for(int j = 0; j < L; j ++) {
            E += - J1 * lattice(i,j) * sum_neighbours(i,j);
        }
    }
    E = (int) (E / 2.0);
}

void class_model::set_M_initial()  {
    M = lattice.sum();
}
