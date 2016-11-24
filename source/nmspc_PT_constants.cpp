//
// Created by david on 10/6/16.
//

#include "nmspc_PT_constants.h"


namespace PT_constants{
    //Job name (used for input and output of files)
    std::string job_name;
    std::string input_file_name;
    std::string model_name;
    std::string output_path;

    //PT class_thermo properties
    double T_min;            //Minimum temperature for thermodynamic quantities
    double T_max;            //Maximum temperature for thermodynamic quantities

    //Lattice Properties
    int J;                   //Interaction strength (coupling)
    int d;                   //Dimension
    int L;                   //Linear size
    int N;                   //Number of spins/particles
    //PT simulation length
    int MCS_warmup;
    int MCS_sample;
    void copy_input(input &in) {
        T_min               = in.get_T_min();
        T_max               = in.get_T_max();
        J                   = in.get_J();
        d                   = in.get_d();
        L                   = in.get_L();
        N                   = in.get_N();
        MCS_warmup          = in.get_MCS_warmup();
        MCS_sample          = in.get_MCS_sample();
        job_name            = in.get_job_name();
        model_name          = in.get_model_name();
        input_file_name     = in.get_input_file();
        output_path         = in.get_output_path();
    }

}

