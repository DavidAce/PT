//
// Created by david on 10/18/16.
//



#include "class_PT_thermo.h"
#include <sim_parameters/n_sim_settings.h>
#include <nmspc_random_numbers.h>


void class_thermo::load_data(std::vector<double>&E_timeseries, std::vector<double>&M_timeseries, double temperature){
    T       = temperature;
    E = Eigen::Map<ArrayXd>(E_timeseries.data(),E_timeseries.size());
    M = Eigen::Map<ArrayXd>(M_timeseries.data(),M_timeseries.size());
}
void class_thermo::load_data(ArrayXd &E_timeseries, ArrayXd &M_timeseries, double temperature){
    T       = temperature;
    E = Eigen::Map<ArrayXd>(E_timeseries.data(),E_timeseries.size());
    M = Eigen::Map<ArrayXd>(M_timeseries.data(),M_timeseries.size());
}

void class_thermo::compute_stats(){
    if (E.size() < 10 or M.size() < 10){
        u = 0;
        m = 0;
        c = 0;
        x = 0;
        sigma_u = 0;
        sigma_m = 0;
        tau_E   = 0;
    }else {
        u = internal_energy(E);
        m = magnetization(M);
        c = specific_heat(E);
        x = susceptibility(M);
        sigma_u = flyvbjerg(E) / settings::model::N;
        sigma_m = flyvbjerg(M) / settings::model::N;
        tau_E = autocorrelation(E);
    }
}

void class_thermo::calc_thermo(){
    if(E.size() <= 1){cout << "Energy hasn't been loaded yet" << endl; return;}
    if(M.size() <= 1){cout << "Magnetization hasn't been loaded yet" << endl; return;}
    if(M.size() != E.size()){cout << "Data size mismatch" << endl; return;}
    ArrayXd u_b(settings::bootstrap::reps);
    ArrayXd m_b(settings::bootstrap::reps);
    ArrayXd c_b(settings::bootstrap::reps);
    ArrayXd x_b(settings::bootstrap::reps);

    tau_E = autocorrelation(E);


    int block_length    = (int) ceil(20*tau_E);
    u_b = bootstrap_overlap_block(E, "u", block_length);
    m_b = bootstrap_overlap_block(M, "m", block_length);
    c_b = bootstrap_overlap_block(E, "c", block_length);
    x_b = bootstrap_overlap_block(M, "x", block_length);

    u = u_b.mean();
    m = m_b.mean();
    c = c_b.mean();
    x = x_b.mean();

    sigma_u = sqrt(variance(u_b));
    sigma_u_flyv = flyvbjerg(E)/settings::model::N;
    sigma_m = sqrt(variance(m_b));
    sigma_c = sqrt(variance(c_b));
    sigma_x = sqrt(variance(x_b));
}

ArrayXd class_thermo::bootstrap_non_overlap_block(const ArrayXd &in, string func, int block_length){
    int num_blocks = (int)(in.size()) / block_length;
    vector<double> boot_avg;
    vector<double>  boot;
    boot_avg.reserve((unsigned long int)settings::bootstrap::reps);
    boot.reserve((unsigned long int)(in.size() + block_length));
    for (int bt = 0; bt < settings::bootstrap::reps; bt++){
        while(boot.size() < (unsigned long int)in.size()){
            int block = rn::uniform_integer(0, num_blocks);
            for (int i = 0; i < block_length; i++){
                boot.push_back(in(i + block*block_length));
            }
        }
        if (func == "u"){ boot_avg.push_back(internal_energy(Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "m"){ boot_avg.push_back(magnetization  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "c"){ boot_avg.push_back(specific_heat  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "x"){ boot_avg.push_back(susceptibility (Map<ArrayXd>(boot.data(), boot.size())));}
        boot.clear();
    }
    return Eigen::Map<ArrayXd>(boot_avg.data(),boot_avg.size());

}

ArrayXd class_thermo::bootstrap_overlap_block(const ArrayXd &in, string func, int block_length){
    vector<double> boot_avg;
    vector<double>  boot;
    boot_avg.reserve((unsigned long int)settings::bootstrap::reps);
    boot.reserve((unsigned long int)(in.size() + block_length) );

    for (int bt = 0; bt < settings::bootstrap::reps; bt++){
        while (boot.size() < (unsigned long int)in.size()){
            int start = rn::uniform_integer(0, (int)in.size());
            for (int i = 0; i < block_length; i++){
                boot.push_back(in(math::mod(i + start, (int) in.size())));
            }
        }
        if (func == "u"){ boot_avg.push_back(internal_energy(Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "m"){ boot_avg.push_back(magnetization  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "c"){ boot_avg.push_back(specific_heat  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "x"){ boot_avg.push_back(susceptibility (Map<ArrayXd>(boot.data(), boot.size())));}
        boot.clear();
    }
    return Eigen::Map<ArrayXd>(boot_avg.data(),boot_avg.size());
}

double class_thermo::autocorrelation(const ArrayXd &A){
    double tau = 0.5*pow(flyvbjerg(A),2.0) / (variance(A)/((int)A.size()-1));
    tau = isnan(tau) || tau < 1 ? 1 : tau;
    return tau;
}

double class_thermo::internal_energy(const ArrayXd &E){
    return E.mean() / settings::model::N;
}

double class_thermo::magnetization(const ArrayXd &M){
    return M.cwiseAbs().mean() / settings::model::N;
}

double class_thermo::specific_heat  (const ArrayXd &E){
    return variance(E)/T/T/settings::model::N;
}

double class_thermo::susceptibility  (const ArrayXd &M){
    return variance(M.cwiseAbs())/T/settings::model::N;
}

double class_thermo::entropy  (const ArrayXd &E){
    return variance(M.cwiseAbs())/T/settings::model::N;
}


double class_thermo::flyvbjerg(const ArrayXd & A){
    ArrayXd B = A;

    int maxBlockingSteps = (int) floor(log(A.rows())/log(2) - 1);
    ArrayXd sigma(maxBlockingSteps), ssigma(maxBlockingSteps);
    for (int i = 0; i < maxBlockingSteps; i++){
        double var    = variance(B);
        sigma(i)      = sqrt(var / (B.size() - 1) ) ;
        ssigma(i)     = sqrt(sigma(i)*sigma(i) * sqrt(2/(B.size()-1)));
        block_transform(B);
    }
    int idx;

    [[maybe_unused]] double dummy = (sigma - ssigma).maxCoeff(&idx);
    return sigma(idx);
}

void class_thermo::block_transform(ArrayXd & B){
    for (int i = 0; i < B.size()/2; i++){
        B(i) = 0.5*(B(2*i) + B(2*i +1));
    }
    B.conservativeResize(B.size()/2);
}

double class_thermo::variance(const ArrayXd &A){
    return A.cwiseAbs2().mean() - pow(A.mean(),2);
}

void class_thermo::reset(){
    u = 0; sigma_u = 0;
    m = 0; sigma_m = 0;
    c = 0; sigma_c = 0;
    x = 0; sigma_x = 0;

    sigma_u_flyv = 0;
    tau_E = 0;
}

