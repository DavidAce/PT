//
// Created by david on 9/28/16.
//

#include "class_PT_input.h"

bool input::folder_exists(const std::string path){
    struct stat info;
    if( info.st_mode & S_IFDIR ) {
        printf("Found directory: %s ", path.c_str());
        return true;
    }
    else {
        printf("Folder does not exist: %s\n", path.c_str());
        return false;
    }
}


bool input::file_exists(const std::string name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}



/** Function that reads in the values in the inputfile **/
/************************************************************************************************/
void input::load_settings_from_file(std::string filename, int world_ID){
    std::ifstream inputFile;
    std::string s_reader, t_reader;
    inputFile.open(filename.c_str());

    if (!inputFile.is_open()){
        if (world_ID == 0) {
            printf("Parameter file not found, exiting\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(1);
        }
    }else {
        if (world_ID == 0) {
            printf("Reading parameters from file %s: ", filename.c_str());
            while (getline(inputFile, s_reader)) {
                std::istringstream r_pick(s_reader);
                r_pick >> t_reader;

                if (t_reader == "T_min=") { r_pick >> T_min; }
                if (t_reader == "T_max=") { r_pick >> T_max; }
                if (t_reader == "J=") { r_pick >> J; }
                if (t_reader == "L=") { r_pick >> L; }
                if (t_reader == "d=") { r_pick >> d; }
                if (t_reader == "MCS_warmup=") { r_pick >> MCS_warmup; }
                if (t_reader == "MCS_sample=") { r_pick >> MCS_sample; }
            }
            N = (int) pow(L, d);
            inputFile.close();
            printf("Success!\n");
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

std::string input::shave_path(std::string filename){
    size_t delim = filename.find_last_of("\\/") + 1;
    size_t npos  = filename.find_last_of(".dat") - delim - 3;
    return filename.substr(delim,npos );
}


/** get and set methods of the private variables defined in parameters.h **/
/************************************************************************************************/

double input::get_T_min()
{
    return T_min;
}
double input::get_T_max()
{
    return T_max;
}

int input::get_J()
{
    return J;
}

int input::get_L()
{
    return L;
}

int input::get_d()
{
    return d;
}

int input::get_N()
{
    return N;
}

int input::get_MCS_warmup()
{
    return MCS_warmup;
}

int input::get_MCS_sample()
{
    return MCS_sample;
}

std::string input::get_filename()
{
    return filename;
}

std::string input::get_job_name()
{
    return job_name;
}




/** Function which broadcasts all parameters to all other MPI processes **/
/************************************************************************************************/
void input::broadcast_parameters(int world_ID){
    MPI_Bcast(&T_min     ,1, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&T_max     ,1, MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&J         ,1, MPI_INT   ,0,MPI_COMM_WORLD);
    MPI_Bcast(&L         ,1, MPI_INT   ,0,MPI_COMM_WORLD);
    MPI_Bcast(&d         ,1, MPI_INT   ,0,MPI_COMM_WORLD);
    MPI_Bcast(&N         ,1, MPI_INT   ,0,MPI_COMM_WORLD);
    MPI_Bcast(&MCS_warmup,1, MPI_INT   ,0,MPI_COMM_WORLD);
    MPI_Bcast(&MCS_sample,1, MPI_INT   ,0,MPI_COMM_WORLD);
    broadcast_string(filename,world_ID);

}



/** Function that takes a string and a rank number and receive or send the string dependning if root or not **/
/*****************************************************************************************/
void input::broadcast_string(std::string& s_broad,int world_ID){

    int s_size;

    if (world_ID == 0){
        s_size = (int)s_broad.size() + 1;
    }

    MPI_Bcast(&s_size,1, MPI_UNSIGNED,0,MPI_COMM_WORLD);

    char c_s_broad[s_size];

    if (world_ID == 0){
        strcpy(c_s_broad, s_broad.c_str());
    }

    MPI_Bcast(c_s_broad,s_size, MPI_CHAR,0,MPI_COMM_WORLD);

    if (world_ID != 0){
        std::string s_tmp(c_s_broad);
        s_broad = s_tmp;
    }
}

