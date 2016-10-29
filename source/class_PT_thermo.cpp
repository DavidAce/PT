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

void class_thermo::compute(){
    if(E.size() <= 1){cout << "Energy hasn't been loaded yet" << endl; return;}
    if(M.size() <= 1){cout << "Magnetization hasn't been loaded yet" << endl; return;}
    if(M.size() != E.size()){cout << "Data size mismatch" << endl; return;}
    ArrayXd u_b(PT_constants::bootstraps);
    ArrayXd m_b(PT_constants::bootstraps);
    ArrayXd c_b(PT_constants::bootstraps);
    ArrayXd x_b(PT_constants::bootstraps);
    tau = autocorrelation(E);
    tau = isnan(tau) ? 1 : tau;
    int n = (int)(E.size()/fmax(1,2*tau));


    ArrayXd E_bootstrap (E.size());
    ArrayXd M_bootstrap (M.size());

    for (int i = 0; i < PT_constants::bootstraps ; i++ ){
        E_bootstrap = rn::random_with_replacement(E,n);
        M_bootstrap = rn::random_with_replacement(M,n);
        u_b(i) = internal_energy(E_bootstrap);
        m_b(i) = magnetization  (M_bootstrap);
        c_b(i) = specific_heat  (E_bootstrap);
        x_b(i) = susceptibility (M_bootstrap);
    }
    u = u_b.mean();
    m = m_b.mean();
    c = c_b.mean();
    x = x_b.mean();

    sigma_u         = sqrt(variance(u_b));
    sigma_u_naive   = sqrt(variance(E))/PT_constants::N / sqrt(n );
    sigma_u_flyv    = flyvbjerg(E)/PT_constants::N;
    sigma_m         = sqrt(variance(m_b));
    sigma_c         = sqrt(variance(c_b));
    sigma_x         = sqrt(variance(x_b));


}


void class_thermo::autocorrelation(){
    if(E.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
    tau = 0.5 * pow(flyvbjerg(E),2) / (variance(E)/(E.size()-1));

}

void class_thermo::internal_energy(){
    if(E.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
    u       = E.mean() / PT_constants::N;
}

void class_thermo::magnetization(){
    if(M.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
    m       = M.cwiseAbs().mean() / PT_constants::N;
}

void class_thermo::specific_heat  (){
    if(E.size() == 0){cout << "Energy hasn't been loaded yet" << endl; exit(1);}
    c       = variance(E)/T/T/PT_constants::N;
}

void class_thermo::susceptibility  (){
    if(M.size() == 0){cout << "Magnetization hasn't been loaded yet" << endl; exit(1);}
    x    = variance(M.cwiseAbs())/T/PT_constants::N;
}

double class_thermo::autocorrelation(const ArrayXd &E){
    return 0.5 * pow(flyvbjerg(E),2) / (variance(E)/(E.size()-1));
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


double class_thermo::flyvbjerg(const ArrayXd & A){
    ArrayXd B = A;

    int maxBlockingSteps = (int) floor(log(A.rows())/log(2) - 1);
    ArrayXd sigma(maxBlockingSteps), ssigma(maxBlockingSteps);
    if (T_ID == 5){cout << "Size: " ;}

    for (int i = 0; i < maxBlockingSteps; i++){
        double var    = variance(B);
        sigma(i)      = sqrt(var / (B.size() - 1) ) ;
        ssigma(i)     = sigma(i) / sqrt(2*(B.size()-1));
        if (T_ID == 5){cout << B.size() << " " ;}
        block_transform(B);
    }
    if (T_ID == 5){cout << endl<< sigma.transpose() << endl;}
    if (T_ID == 5){cout << ssigma.transpose() << endl;}
    int idx;
    double dummy = (sigma - ssigma).maxCoeff(&idx);
    if (T_ID == 5){cout << "Result: " << sigma(idx) << endl;}
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