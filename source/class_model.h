//
// Created by david on 2016-07-24.
//

#ifndef CLASS_MODEL_H
#define CLASS_MODEL_H
#include <Eigen/Core>
#include <Eigen/Dense>
#include "nmspc_random_numbers.h"
#include "nmspc_math_algorithms.h"
using namespace Eigen;
using namespace std;

class class_model {
private:
    int randI,randJ; //Coordinates of random position;
    int L; //Linear size of your model
public:
    class_model(int l, int j): L(l), J(j) {
        lattice.resize(L,L);
        randomize_lattice();
    };
    ArrayXXi lattice;                           //The Lattice Data structure
    //double E_new, M_new;                      //Store values from MC-trial
    const bool discrete_model = true;           //Toggle model type
    const int J;                                //Interaction strength

    void randomize_lattice();
    inline void  __attribute__((always_inline)) flip(){
        lattice(randI,randJ) *= -1;
    }

    double get_E();
    double get_M();

    int __attribute__((always_inline)) sum_neighbours(const int i, const int j){
        return            lattice(math::mod(i+1,L), math::mod(j  ,L))
                     +    lattice(math::mod(i  ,L), math::mod(j+1,L))
                     +    lattice(math::mod(i-1,L), math::mod(j  ,L))
                     +    lattice(math::mod(i  ,L), math::mod(j-1,L));
    }

    inline void __attribute__((always_inline)) make_new_state(const double E, const double M, double &E_trial, double &M_trial){
        randI = rn::uniform_integer(0, L-1);
        randJ = rn::uniform_integer(0, L-1);
        E_trial = E + J * 2 * lattice(randI, randJ) * sum_neighbours(randI, randJ);
        M_trial = M - 2 *     lattice(randI, randJ);
    }

    friend std::ostream &operator<<(std::ostream &, const class_model &);
};


#endif //WL_CLASS_MODEL_H
