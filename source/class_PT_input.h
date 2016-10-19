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
    std::string filename;
    std::string job_name;
    bool        job_given;
public:
    input(std::string &name, int world_ID): filename(name){
        if (filename.empty()){
            job_given = false;
            job_name  = "default";
            load_settings_from_file("input/default.dat",world_ID);
        }else{
            job_given           = true;
            job_name            = shave_path(filename);
            load_settings_from_file(filename, world_ID);

        }
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
    std::string get_filename();
    std::string get_job_name();
    void        broadcast_parameters(int world_ID);
    std::string shave_path(std::string filename);

    void load_settings_from_file(std::string filename,int world_ID);

    bool folder_exists(const std::string path);
    bool file_exists(const std::string name);

};


#endif //PT_CLASS_PT_INPUT_H
