//
// Created by david on 10/18/16.
//

#include "class_PT_thermo.h"

void class_thermo::load_data(int temperature_ID, double temperature){
    T       = temperature;
    T_ID    = temperature_ID;
    E       = read_file("output/" + PT_constants::job_name + "/timeseries/E" + to_string(T_ID) + ".dat");
    M       = read_file("output/" + PT_constants::job_name + "/timeseries/M" + to_string(T_ID) + ".dat");
}

void class_thermo::compute_stats(){
    E_mean      = E.mean();
    E_sq_mean   = E.cwiseAbs2().mean();

    M_mean      = M.cwiseAbs().mean();
    M_sq_mean   = M.cwiseAbs2().mean();

    E_var       = E_sq_mean - E_mean*E_mean;
    E_std       = sqrt(E_var);

    M_var       = M_sq_mean - M_mean*M_mean;
    M_std       = sqrt(M_var);

}

void class_thermo::compute_thermo(){
    if(E.size() <= 1){cout << "Energy hasn't been loaded yet" << endl; return;}
    if(M.size() <= 1){cout << "Magnetization hasn't been loaded yet" << endl; return;}
    if(M.size() != E.size()){cout << "Data size mismatch" << endl; return;}
    ArrayXd u_b(PT_constants::bootstraps);
    ArrayXd m_b(PT_constants::bootstraps);
    ArrayXd c_b(PT_constants::bootstraps);
    ArrayXd x_b(PT_constants::bootstraps);
//    ArrayXd s_b(PT_constants::bootstraps);

    tau_E = autocorrelation(E);
//    tau_M = autocorrelation(M);
    tau_E = isnan(tau_E) || tau_E < 1 ? 1 : tau_E;
//    tau_M = isnan(tau_M) || tau_M < 1 ? 1 : tau_M;

    int block_length    = (int) ceil(20*tau_E);
//    int n_independent   = (int)(E.size()) / block_length;
    u_b = bootstrap_overlap_block(E, "u", block_length);
    m_b = bootstrap_overlap_block(M, "m", block_length);
    c_b = bootstrap_overlap_block(E, "c", block_length);
    x_b = bootstrap_overlap_block(M, "x", block_length);
//    s_b = bootstrap_overlap_block(M, "s", block_length);

    u = u_b.mean();
    m = m_b.mean();
    c = c_b.mean();
    x = x_b.mean();
//    s = s_b.mean();
    sigma_u_flyv = flyvbjerg(E)  /  PT_constants::N;
    sigma_u = sqrt(variance(u_b));
    sigma_m = sqrt(variance(m_b));
    sigma_c = sqrt(variance(c_b));
    sigma_x = sqrt(variance(x_b));
//    sigma_s = sqrt(variance(s_b));

//
//    int block_length    = (int) ceil(2*tau_E);
//    int n_independent   = (int)(E.size()) / block_length;
//    ArrayXd E_bootstrap (E.size());
//    ArrayXd M_bootstrap (M.size());
//    for (int i = 0; i < PT_constants::bootstraps ; i++ ){
//        E_bootstrap = rn::random_with_replacement(E, n_independent);
//        M_bootstrap = rn::random_with_replacement(M, n_independent);
//        u_b(i) = internal_energy(E_bootstrap);
//        m_b(i) = magnetization  (M_bootstrap);
//        c_b(i) = specific_heat  (E_bootstrap);
//        x_b(i) = susceptibility (M_bootstrap);
//    }
//
//
//
//    sigma_u = sqrt(variance(u_b));
//    sigma_m = sqrt(variance(m_b));
//    sigma_c = sqrt(variance(c_b));
//    sigma_x = sqrt(variance(x_b));
//
//
//
//    block_length    = (int) ceil(20*tau_E);
//    n_independent   = (int)(E.size()) / block_length;
//
//    u_b = bootstrap_non_overlap_block(E, "u", block_length);
//    m_b = bootstrap_non_overlap_block(M, "m", block_length);
//    c_b = bootstrap_non_overlap_block(E, "c", block_length);
//    x_b = bootstrap_non_overlap_block(M, "x", block_length);
//
//    sigma_u_tau   = sqrt(variance(u_b));
//    sigma_m_tau   = sqrt(variance(m_b));
//    sigma_c_tau   = sqrt(variance(c_b));
//    sigma_x_tau   = sqrt(variance(x_b));



}

ArrayXd class_thermo::bootstrap_non_overlap_block(const ArrayXd &in, string func, int block_length){
    int num_blocks = (int)(in.size()) / block_length;
    vector<double> boot_avg;

    for (int bt = 0; bt < PT_constants::bootstraps; bt++){
        vector<double> boot;
        while(boot.size() < in.size()){
            int block = rn::uniform_integer(0, num_blocks);
            for (int i = 0; i < block_length; i++){
                boot.push_back(in(i + block*block_length));
            }
        }
        if (func == "u"){ boot_avg.push_back(internal_energy(Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "m"){ boot_avg.push_back(magnetization  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "c"){ boot_avg.push_back(specific_heat  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "x"){ boot_avg.push_back(susceptibility (Map<ArrayXd>(boot.data(), boot.size())));}
//        if (func == "s"){ boot_avg.push_back(entropy        (Map<ArrayXd>(boot.data(), boot.size())));}
    }
    return Eigen::Map<ArrayXd>(boot_avg.data(),boot_avg.size());

}

ArrayXd class_thermo::bootstrap_overlap_block(const ArrayXd &in, string func, int block_length){
    vector<double> boot_avg;
    for (int bt = 0; bt < PT_constants::bootstraps; bt++){
        vector<double> boot;
        while (boot.size() < in.size()){
            int start = rn::uniform_integer(0, (int)in.size());
            for (int i = 0; i < block_length; i++){
                boot.push_back(in(math::mod(i + start, (int) in.size())));
            }
        }
        if (func == "u"){ boot_avg.push_back(internal_energy(Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "m"){ boot_avg.push_back(magnetization  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "c"){ boot_avg.push_back(specific_heat  (Map<ArrayXd>(boot.data(), boot.size())));}
        if (func == "x"){ boot_avg.push_back(susceptibility (Map<ArrayXd>(boot.data(), boot.size())));}
//        if (func == "s"){ boot_avg.push_back(entropy        (Map<ArrayXd>(boot.data(), boot.size())));}
    }
    return Eigen::Map<ArrayXd>(boot_avg.data(),boot_avg.size());
}

double class_thermo::autocorrelation(const ArrayXd &A){
    return 0.5*pow(flyvbjerg(A),2) / (variance(A)/(A.size()-1));
}

double class_thermo::internal_energy(const ArrayXd &E){
    return E.mean() / PT_constants::N;
}

double class_thermo::magnetization(const ArrayXd &M){
    return M.cwiseAbs().mean() / PT_constants::N;
}

double class_thermo::specific_heat  (const ArrayXd &E){
    return variance(E)/T/T/PT_constants::N;
}

double class_thermo::susceptibility  (const ArrayXd &M){
    return variance(M.cwiseAbs())/T/PT_constants::N;
}

double class_thermo::entropy  (const ArrayXd &E){
    return variance(M.cwiseAbs())/T/PT_constants::N;
}


double class_thermo::flyvbjerg(const ArrayXd & A){
    ArrayXd B = A;

    int maxBlockingSteps = (int) floor(log(A.rows())/log(2) - 1);
    ArrayXd sigma(maxBlockingSteps), ssigma(maxBlockingSteps);
    for (int i = 0; i < maxBlockingSteps; i++){
        double var    = variance(B);
        sigma(i)      = sqrt(var / (B.size() - 1) ) ;
//        ssigma(i)     = sigma(i) / sqrt(2*(B.size()-1));
        ssigma(i)     = sqrt(sigma(i)*sigma(i) * sqrt(2/(B.size()-1)));
//        if (T_ID == 5){cout << B.size() << " " ;}
        block_transform(B);
    }
    int idx;

    double dummy = (sigma - ssigma).maxCoeff(&idx);
//    if (T_ID == 5){cout << "Result: " << sigma(idx) << endl;}
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
    u = 0; sigma_u = 0;// sigma_u_tau = 0; sigma_u_tau2 = 0;
    m = 0; sigma_m = 0;// sigma_m_tau = 0; sigma_m_tau2 = 0;
    c = 0; sigma_c = 0;// sigma_c_tau = 0; sigma_c_tau2 = 0;
    x = 0; sigma_x = 0;// sigma_x_tau = 0; sigma_x_tau2 = 0;

    sigma_u_flyv = 0;
    tau_E = 0; //tau_M = 0;
}

ArrayXXd class_thermo::read_file(string filename) {
    ifstream infile;
    vector<string> lines;
    string line;
    string number;
    infile.open(filename);

    if (!infile.is_open()) {
        cout << "Can't open: " << filename << endl;
        return ArrayXXd::Zero(1,1);
//        MPI_Finalize();
//        exit(5);
    }
    unsigned long int rows = 0, cols = 0;
    //Load all the lines first and count rows.
    while (getline(infile,line)) {
        lines.push_back(line);
        rows++;
    }
    //Now count the number of elements on the  first line
    stringstream stream(lines[0]);
    while (!stream.eof()) {
        stream >> number;
        cols++;
    }
    //Now make your matrix and fill it with the contents of line[].
    ArrayXXd result(rows, cols);
    int j;
    for (unsigned long int i = 0; i < rows; i++) {
        stringstream new_stream;
        new_stream << lines[i];
        j = 0;
        while (!new_stream.eof()) {
            new_stream >> number;
            result(i, j++) = std::stod(number);
        }
    }
    return result;
}






//
//void class_thermo::autocorrelation_E(){
//    if(E.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
//    tau_E = 0.5*pow(flyvbjerg(E),2) / (variance(E)/(E.size()-1));
//
//}
//
//void class_thermo::autocorrelation_M(){
//    if(M.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
//    tau_M = 0.5*pow(flyvbjerg(M),2) / (variance(M)/(M.size()-1));
//
//}
//
//void class_thermo::internal_energy(){
//    if(E.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
//    u       = E.mean() / PT_constants::N;
//}
//
//void class_thermo::magnetization(){
//    if(M.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
//    m       = M.cwiseAbs().mean() / PT_constants::N;
//}
//
//void class_thermo::specific_heat  (){
//    if(E.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
//    c       = variance(E)/T/T/PT_constants::N;
//}
//
//void class_thermo::susceptibility  (){
//    if(M.size() == 0){cout << "Magnetization hasn't been loaded yet" << endl; exit(1);}
//    x    = variance(M.cwiseAbs())/T/PT_constants::N;
//}