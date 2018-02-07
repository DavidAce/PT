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
#include <sim_parameters/n_sim_settings.h>
#include <IO/class_hdf5_file.h>

#include <fstream>
#include <IO/class_hdf5_table_buffer.h>


using namespace std;
using namespace Eigen;


class output {
private:

    const string      timeseries_group     = "timeseries";
    const string      groundstates_group   = "groundstates";
    const string      thermodynamics_group = "thermodynamics";
    int         iteration;
    int         precision = 12;
    int         world_ID;

public:
    std::shared_ptr<class_hdf5_file> hdf5;
    std::shared_ptr<class_hdf5_table_buffer> table_buffer;
    bool thermodynamics_has_data = false;
    bool timeseries_has_data = false;
    explicit output(int id);

    //Groundstate Lattices
    template<typename T>
    void store_groundstates(T &lattice_groundstate){
        hdf5->write_dataset_mpi(lattice_groundstate, groundstates_group + "/" + "lattice");
        lattice_groundstate.clear();
    }


    //Generic data
    template<typename T>
    void store_thermo(const T &data, std::string name){
        hdf5->write_dataset_mpi(data, thermodynamics_group + "/" + name);
        thermodynamics_has_data = true;
    }


    //Time series
    template<typename T>
    void store_samples(const T &data, std::string name, int store_counter){
        hdf5->write_dataset_mpi(data, timeseries_group + "/" + name);
        timeseries_has_data = true;
    }

    //Anything
    template<typename T>
    void store_matrix(const T &data, std::string name, int store_counter){
        hdf5->write_dataset_mpi(data,  name);
    }
    
    

//
//    template<typename T>
//    void write_dataset(const T &data, string filename){
//        ofstream file(filename,ios::out | ios::trunc);
//        file << fixed << showpoint << setprecision(precision);
//        file << data << endl;
//        file.flush();
//        file.close();
//    }
//
//    template<typename Derived>
//    void write_dataset(const MatrixBase<Derived> &data, string filename){
//        ofstream file(filename,ios::out | ios::trunc);
//        file << fixed << showpoint << setprecision(precision);
//        string      _coeffSeparator = "	";
//        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
//        file << data.format(fmt) << endl;
//        file.flush();
//        file.close();
//    }
//
//    template<typename Derived>
//    void write_dataset(const ArrayBase<Derived> &data, string filename){
//        ofstream file(filename,ios::out | ios::trunc);
//        file << fixed << showpoint << setprecision(precision);
//        string      _coeffSeparator = "	";
//        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
//        file << data.format(fmt) << endl;
//        file.flush();
//        file.close();
//    }
//
//    template<typename Derived>
//    void append_to_file(const MatrixBase<Derived> &data, string filename){
//        ofstream file(filename,ios::out | ios::app);
//        file << fixed << showpoint << setprecision(precision);
//        string      _coeffSeparator = "	";
//        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
//        file << data.format(fmt) << endl;
//        file.flush();
//        file.close();
//    }
//
//    template<typename Derived>
//    void append_to_file(const ArrayBase<Derived> &data, string filename){
//        ofstream file(filename,ios::out | ios::app);
//        file << fixed << showpoint << setprecision(precision);
//        string      _coeffSeparator = "	";
//        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
//        file << data.format(fmt) << endl;
//        file.flush();
//        file.close();
//    }

};



#endif //WL_CLASS_PRINT_DATA_H
