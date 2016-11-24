//
// Created by david on 9/28/16.
//

#ifndef PT_CLASS_PT_INPUT_H
#define PT_CLASS_PT_INPUT_H
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <cstdlib>
#include <mpi.h>
#include <math.h>
class input {

private:
    double T_min;
    double T_max;
    int J;

    int L;
    int d;
    int N;

    int MCS_warmup;
    int MCS_sample;


    void broadcast_string(std::string&,int);
    std::string job_name;
    std::string model_name;
    std::string input_file_name;
    std::string output_path;

public:
    input(std::string mname, std::string fname, int world_ID): input_file_name(fname), model_name(mname){
        if (input_file_name.empty()){
            job_name        = "default";
            model_name      = "default";
            input_file_name = "input/default/default.dat";
        }

        job_name            = shave_path(input_file_name);
        output_path         = "output/" + model_name + "/" + job_name + "/";
        load_settings_from_file(input_file_name, world_ID);
        broadcast_parameters(world_ID);
    }
    double      get_T_min();
    double      get_T_max();
    int         get_J();

    int         get_L();
    int         get_d();
    int         get_N();
    int         get_MCS_warmup();
    int         get_MCS_sample();
    std::string get_input_file();
    std::string get_job_name();
    std::string get_model_name();
    std::string get_output_path();
    void        broadcast_parameters(int world_ID);
    std::string shave_path(std::string filename);

    void load_settings_from_file(std::string filename,int world_ID);

    bool folder_exists(const std::string path);
    bool file_exists(const std::string name);
};


#endif //PT_CLASS_PT_INPUT_H
