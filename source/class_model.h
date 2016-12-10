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

template <typename T>
class observable{
public:
    T value;
    MPI_Datatype mpi_type;
    observable(){
        if(std::is_same<T,int>::value)      {mpi_type = MPI_INT;}
        if(std::is_same<T,double>::value)   {mpi_type = MPI_DOUBLE;}
    }

};



template <typename T>
class Ising {

private:
    int randI,randJ; //Coordinates of random position;

public:
    int L;            //Linear size of your model
    T J1;             //Coupling 1
    T J2;             //Coupling 2
    observable<T> E;
    observable<T> E_trial;        //Energy
    observable<T> M, M_trial;     //Magnetization
    observable<T> O, O_trial;     //Other order parameter

    ArrayXXi lattice;

    Ising(int l, T j1, T j2): L(l), J1(j1), J2(j2){
        lattice.resize(L,L);
        randomize_lattice();
        set_E_initial();
        set_M_initial();
    };

    inline void randomize_lattice() __attribute__((always_inline)) {
        for(int i = 0; i < L ; i++){
            for(int j= 0; j < L ; j++){
                lattice(i,j) = 2 * rn::uniform_integer_1() - 1;
            }
        }
    }

    inline void flip() __attribute__((always_inline)) {
        lattice(randI,randJ) *= -1;
    }

    inline int sum_neighbours(const int i, const int j) __attribute__((always_inline)){
        return            lattice(math::mod(i+1,L), math::mod(j  ,L))
                          +    lattice(math::mod(i  ,L), math::mod(j+1,L))
                          +    lattice(math::mod(i-1,L), math::mod(j  ,L))
                          +    lattice(math::mod(i  ,L), math::mod(j-1,L));
    }

    inline void make_new_state()__attribute__((always_inline)) {
        randI = rn::uniform_integer(0, L-1);
        randJ = rn::uniform_integer(0, L-1);
        E_trial.value = E.value + J1 * 2 *  lattice(randI, randJ) * sum_neighbours(randI, randJ);
        M_trial.value = M.value - 2 *       lattice(randI, randJ);
    };



    inline void accept_state()  __attribute__ ((always_inline)) {
        E.value = E_trial.value;
        M.value = M_trial.value;
        flip();
    }


    void set_E_initial(){
        E.value = 0;
        for(int i = 0; i < L; i ++){
            for(int j = 0; j < L; j ++) {
                E.value += - J1 * lattice(i,j) * sum_neighbours(i,j);
            }
        }
        E.value = (T) (E.value / 2.0);
    }

    void set_M_initial(){
        M.value = lattice.sum();
    }

    friend std::ostream &operator<<(std::ostream & os, const Ising & ising){
        os << ising.lattice << std::endl;
        return os;
    }

};



#endif //WL_CLASS_MODEL_H
