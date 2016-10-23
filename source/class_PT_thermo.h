//
// Created by david on 10/18/16.
//

#ifndef PT_CLASS_PT_THERMO_H
#define PT_CLASS_PT_THERMO_H
#include "nmspc_PT_constants.h"
#include "nmspc_random_numbers.h"
#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

class class_thermo{
private:
    ArrayXXd read_file(string);
    ArrayXd E,M;


public:
    class_thermo() {};
    double T;
    int    T_ID;


    double u, sigma_u;
    double m, sigma_m;
    double c, sigma_c;
    double x, sigma_x;

    double sigma_u_naive;
    double sigma_u_flyv;

    double tau;


    void   load_data(int temperature_ID, double temperature);
    void   compute();
    double flyvbjerg  (const ArrayXd & A); //Computes the effective standard deviation of a correlated series
    void   block_transform  (ArrayXd & B);

    void   autocorrelation      ();
    void   internal_energy      ();
    void   magnetization        ();
    void   specific_heat        ();
    void   susceptibility       ();
    double autocorrelation      (const ArrayXd &E);
    double internal_energy      (const ArrayXd &E);
    double magnetization        (const ArrayXd &E);
    double specific_heat        (const ArrayXd &E);
    double susceptibility       (const ArrayXd &M);

    double variance             (const ArrayXd &A);

};



#endif //PT_CLASS_PT_THERMO_H
