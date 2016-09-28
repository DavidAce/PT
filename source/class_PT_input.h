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
//This class reads settings that may exist in the folder named input/
namespace PT_settings{
    //Model settings
    int L;
    double J1;
    double J2;
    //Simulation settings
    int warmup_MCS;
    int sample_MCS;

}



class input {
private:
    bool folder_existence_check();
    bool file_existence_check();
    bool file_existence_check(const std::string filename);
    int  file_counter();
    int  num_files_in_folder;
    bool folder_exists;
    bool file_exists;

    std::string pathname;
    std::string filename;

public:
    input(int job): job_id(job){
        pathname = "input/";
        if (job_id >= 0){
            job_given           = true;
            job_name            = std::to_string(job_id);
            folder_exists       = folder_existence_check();
            filename            = pathname + job_name + ".dat";
            load_settings_from_file();
        }else{
            job_given = false;
            job_name  = "default";
            load_settings_default();
        }
    }
    bool        job_given;
    int         job_id;
    std::string job_name;


    void load_settings_default();
    void load_settings_from_file();
};


#endif //PT_CLASS_PT_INPUT_H
