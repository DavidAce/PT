//
// Created by david on 2016-07-26.
//

#ifndef WL_CLASS_PRINT_DATA_H
#define WL_CLASS_PRINT_DATA_H
#include <Eigen/Core>
#include <Eigen/Dense>
#include <cstdlib>
#include <stdexcept>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>
#include <iostream>
#include <iomanip>
#include "nmspc_PT_constants.h"


using namespace std;
using namespace Eigen;
//namespace fs = std::experimental::filesystem;

class output {
private:

    string      folder;
    int         iteration;
    int         precision = 12;
    int         world_ID;
public:
    output(int id):world_ID(id){};
    void create_folder(string folder_name);
    void create_folder_master(const int &id);
    void create_folder_worker();
    int mkdir_p(const char *path);

    //File streams
    template<typename Derived>
    void store_samples(const ArrayBase<Derived> &data, std::string name, int store_counter){
        if (store_counter == 0){
            folder ="output/"  + PT_constants::job_name + "/timeseries/";
            create_folder_master(world_ID);
            write_to_file(data, folder + name);
        }else{
            write_to_file(data, folder + name);
        }
    }

    template<typename Derived>
    void write_to_file(const MatrixBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.close();
    }
    template<typename Derived>
    void write_to_file(const ArrayBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.close();
    }

    template<typename Derived>
    void append_to_file(const MatrixBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::app);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.close();
    }
    template<typename Derived>
    void append_to_file(const ArrayBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::app);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.close();
    }

};



#endif //WL_CLASS_PRINT_DATA_H
