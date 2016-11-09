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
    double E_mean, E_sq_mean, M_mean, M_sq_mean;
    double E_var , E_std, M_var, M_std;



public:
    class_thermo() {};
    double T;
    int    T_ID;

    double u, sigma_u;// sigma_u_tau, sigma_u_tau2;
    double m, sigma_m;// sigma_m_tau, sigma_m_tau2;
    double c, sigma_c;// sigma_c_tau, sigma_c_tau2;
    double x, sigma_x;// sigma_x_tau, sigma_x_tau2;
//    double s, sigma_s;
    double sigma_u_flyv;
    double tau_E;// tau_M;

    void   reset();
    void   load_data(int temperature_ID, double temperature);
    void   compute_stats();
    void   compute_thermo();

    double flyvbjerg  (const ArrayXd & A); //Computes the effective standard deviation of a correlated series
    void   block_transform  (ArrayXd & B);

    double autocorrelation      (const ArrayXd &E);
    double internal_energy      (const ArrayXd &E);
    double magnetization        (const ArrayXd &M);
    double specific_heat        (const ArrayXd &E);
    double susceptibility       (const ArrayXd &M);
    double entropy              (const ArrayXd &E);
    ArrayXd bootstrap_non_overlap_block(const ArrayXd &in, string func, int block_length);
    ArrayXd bootstrap_overlap_block    (const ArrayXd &in, string func, int block_length);
    double variance             (const ArrayXd &A);

};



#endif //PT_CLASS_PT_THERMO_H
