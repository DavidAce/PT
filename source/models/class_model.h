//
// Created by david on 2016-07-24.
//

#ifndef CLASS_MODEL_H
#define CLASS_MODEL_H
#include <mpi.h>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <nmspc_random_numbers.h>
#include <nmspc_math_algorithms.h>
#include <sim_parameters/n_sim_settings.h>

class class_model {
public:

    class_model(){
        cout << "J1: "<< settings::model::J1 << endl<< flush;

        J1  = settings::model::J1;
        on  = settings::model::on;

        MPI_Barrier(MPI_COMM_WORLD);
        cout << "Setting model size: "<< endl<< flush;

        L     = settings::model::L;
        N     = L*L;
        lattice.resize(L,L);
        MPI_Barrier(MPI_COMM_WORLD);
        cout << "Random numbers initializing "<< endl<< flush;
        randI = rn::uniform_integer(0,L-1);
        randJ = rn::uniform_integer(0,L-1);

        MPI_Barrier(MPI_COMM_WORLD);
        cout << "Randomizing lattice: " << endl<< flush;

        randomize_lattice();
        MPI_Barrier(MPI_COMM_WORLD);
        cout << "Finding initial energy: " << endl<< flush;

        set_E_initial();
        MPI_Barrier(MPI_COMM_WORLD);
        cout << "Finding initial magnetization: " << endl<< flush;

        set_M_initial();
        MPI_Barrier(MPI_COMM_WORLD);

        cout << "Finished constructor" << endl<< flush;

    };

    Eigen::ArrayXXi lattice;
    int L;           //Linear size of your model
    int N;
    int randI,randJ; //Coordinates of random position;

    double E;
    double E_trial;        //Energy
    double M, M_trial;     //Magnetization
    double O, O_trial;     //Other order parameter

    double  J1   = 1;                        /*!< Nearest (Ferromagnetic) coupling        */
    double  J2   = 0;                        /*!< Next nearest coupling (NOT IMPLEMENTED) */
    double  J3   = 0;                        /*!< Third nearest coupling (NOT IMPLEMENTED) */
    double  h    = 0;                        /*!< Magnetic field bias */

    bool on = true;


    inline void randomize_lattice()  __attribute__((always_inline)) {
        for(int i = 0; i < L ; i++){
            for(int j= 0; j < L ; j++){
                lattice(i,j) = 2 * rn::uniform_integer_1() - 1;
            }
        }
    }

    inline void flip()  __attribute__((always_inline)) {
        lattice(randI,randJ) *= -1;
    }

    inline int sum_neighbours(const int i, const int j)  __attribute__((always_inline)){
        return            lattice(math::mod(i+1,L), math::mod(j  ,L))
                         +lattice(math::mod(i  ,L), math::mod(j+1,L))
                         +lattice(math::mod(i-1,L), math::mod(j  ,L))
                         +lattice(math::mod(i  ,L), math::mod(j-1,L));
    }

    inline void make_new_state()  __attribute__((always_inline)) {
        randI = rn::uniform_integer(0, L-1);
        randJ = rn::uniform_integer(0, L-1);
        E_trial = E + J1 * 2 *  lattice(randI, randJ) * sum_neighbours(randI, randJ);
        M_trial = M - 2 * lattice(randI, randJ);
    };



    inline void accept_state()  __attribute__ ((always_inline)) {
        E = E_trial;
        M = M_trial;
        flip();
    }


    void set_E_initial()  {
        E = 0;
        for(int i = 0; i < L; i ++){
            for(int j = 0; j < L; j ++) {
                E += - J1 * lattice(i,j) * sum_neighbours(i,j);
            }
        }
        E = (int) (E / 2.0);
    }

    void set_M_initial()  {
        M = lattice.sum();
    }

    friend std::ostream &operator<<(std::ostream & os, const class_model & model){
        os << model.lattice << std::endl;
        return os;
    }

};



#endif //WL_CLASS_MODEL_H
