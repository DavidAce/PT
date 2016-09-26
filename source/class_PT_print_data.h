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
#include "class_PT_worker.h"



using namespace std;
using namespace Eigen;
//namespace fs = std::experimental::filesystem;

class outdata {
private:
    string      folder;
    int         iteration;
    int         precision = 12;
public:
    outdata();
    void create_folder(string folder_name);
    void create_iteration_folder_master(const int &iter, const int &id);
    void create_iteration_folder_worker(const int &iter);
    int mkdir_p(const char *path);
    //File streams

    void write_data_worker(class_worker &);
    void write_data_master(class_worker &);

    template<typename Derived>
    void write_to_file(const MatrixBase<Derived> &data, string &filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, DontAlignCols, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.close();
    }
    template<typename Derived>
    void write_to_file(const ArrayBase<Derived> &data, string &filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, DontAlignCols, _coeffSeparator);;
        file << data.format(fmt) << endl;
        file.close();
    }

};



#endif //WL_CLASS_PRINT_DATA_H
