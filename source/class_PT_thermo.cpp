//
// Created by david on 10/18/16.
//

#include "class_PT_thermo.h"

void thermodynamics::load_data(){
    E       = read_file("output/" + PT_constants::job_name + "/timeseries/E" + to_string(world_ID) + ".dat");
    M       = read_file("output/" + PT_constants::job_name + "/timeseries/E" + to_string(world_ID) + ".dat");
}

void thermodynamics::bootstrap_error(){
    for (int i = 0; i < PT_constants::bootstraps ; i++ ){

    }


}





void thermodynamics::internal_energy(){
    u       = E.mean() / PT_constants::N;
}

void thermodynamics::specific_heat  (){
    c       = (pow(E.mean(),2) - (E*E).mean())/T/T/PT_constants::N;
}

void thermodynamics::susceptibility  (){
    x       = (pow(M.mean(),2) - (M*M).mean())/T/PT_constants::N;
}

double thermodynamics::internal_energy(const ArrayXd &E){
    return E.mean() / PT_constants::N;
}

double thermodynamics::specific_heat  (const ArrayXd &E){
    return (pow(E.mean(),2) - (E*E).mean())/T/T/PT_constants::N;
}

double thermodynamics::susceptibility  (const ArrayXd &M){
    return (pow(M.mean(),2) - (M*M).mean())/T/PT_constants::N;
}




double thermodynamics::flyvbjerg_error(const ArrayXd & A){
    ArrayXd B = A;

    int maxBlockingSteps = (int) floor(log(A.rows())/log(2) - 1);
    ArrayXd error(maxBlockingSteps), errorr(maxBlockingSteps);

    for (int i = 0; i < maxBlockingSteps; i++){
        error(i)      =   ( (pow(A.mean(),2) - (A*A).mean())/(A.size() - 1) );
        errorr(i)     =    ( sqrt(2* pow((pow(A.mean(),2) - (A*A).mean()),2)/pow(A.size() - 1, 3) ));
        block_transform(B);
    }
    return (error - errorr).maxCoeff();
}


void thermodynamics::block_transform(ArrayXd & B){
    for (int i = 0; i < B.size()/2; i++){
        B(i) = 0.5*(B(2*i) + B(2*i +1));
    }
    B.conservativeResize(B.size()/2);
}

ArrayXXd thermodynamics::read_file(string filename) {
    ifstream infile;
    vector<string> lines;
    string line;
    string number;
    infile.open(filename);

    if (!infile.is_open()) {
        cout << "Could not open file with name: " << filename << endl;
        MPI_Finalize();
        exit(5);
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