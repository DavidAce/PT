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


    double u, sigma_u, sigma_u_tau, sigma_u_tau2;
    double m, sigma_m, sigma_m_tau, sigma_m_tau2;
    double c, sigma_c, sigma_c_tau, sigma_c_tau2;
    double x, sigma_x, sigma_x_tau, sigma_x_tau2;

    double sigma_u_flyv;

    double tau_E, tau_M;


    void   load_data(int temperature_ID, double temperature);
    void   compute();
    double flyvbjerg  (const ArrayXd & A); //Computes the effective standard deviation of a correlated series
    void   block_transform  (ArrayXd & B);

//    void   autocorrelation_E      ();
//    void   autocorrelation_M      ();
//    void   internal_energy      ();
//    void   magnetization        ();
//    void   specific_heat        ();
//    void   susceptibility       ();
    double autocorrelation      (const ArrayXd &E);
    double internal_energy      (const ArrayXd &E);
    double magnetization        (const ArrayXd &M);
    double specific_heat        (const ArrayXd &E);
    double susceptibility       (const ArrayXd &M);
    ArrayXd bootstrap           (const ArrayXd &in, string func, int block_length );
    ArrayXd bootstrap2           (const ArrayXd &in, string func, int block_length );

    double variance             (const ArrayXd &A);

};



#endif //PT_CLASS_PT_THERMO_H
