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

    class_model();

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


    void randomize_lattice();
    void flip()                         __attribute__ ((hot));
    int sum_neighbours(int i, int j)    __attribute__ ((hot));
    void make_new_state()               __attribute__ ((hot));
    void accept_state()                 __attribute__ ((hot));
    void set_E_initial();
    void set_M_initial();
    friend std::ostream &operator<<(std::ostream & os, const class_model & model){
        os << model.lattice << std::endl;
        return os;
    }

};



#endif //WL_CLASS_MODEL_H
