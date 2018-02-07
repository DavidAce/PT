//
// Created by david on 10/18/16.
//

#ifndef PT_CLASS_PT_THERMO_H
#define PT_CLASS_PT_THERMO_H

#include <Eigen/Core>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

class class_thermo{
private:
    ArrayXd E,M;

public:
    class_thermo() {};
    double T;

    double u = 0; double sigma_u = 0;
    double m = 0; double sigma_m = 0;
    double c = 0; double sigma_c = 0;
    double x = 0; double sigma_x = 0;
    double sigma_u_flyv = 0;
    double tau_E = 0;// tau_M;

    void   reset();
    void   load_data(std::vector<double>&E_timeseries, std::vector<double>&M_timeseries, double temperature);
    void   load_data(ArrayXd &E_timeseries, ArrayXd &M_timeseries, double temperature);
    void   compute_stats();
    void   calc_thermo();

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
