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
#include <fstream>


using namespace std;
using namespace Eigen;

class output {
private:

    string      timeseries_folder     ;
    string      groundstates_folder   ;
    string      thermodynamics_folder ;
    int         iteration;
    int         precision = 12;
    int         world_ID;
public:
    output(int id):world_ID(id){
        //Create folders
        timeseries_folder       = settings::hdf5::output_folder + "timeseries/";
        groundstates_folder     = settings::hdf5::output_folder + "groundstates/";
        thermodynamics_folder   = settings::hdf5::output_folder + "thermodynamics/";
        create_folder_master(timeseries_folder    , world_ID);
        create_folder_master(groundstates_folder  , world_ID);
        create_folder_master(thermodynamics_folder, world_ID);
    };
    void create_folder(string folder_name);
    void create_folder_master(string folder, const int &id);
    void create_folder_worker(string folder);
    int mkdir_p(const char *path);


    //Groundstate Lattices
    template<typename T>
    void store_groundstates(vector<T> &lattice_groundstate){
        if (world_ID == 0) {
            for (int i = 0; i < (int)lattice_groundstate.size(); i++) {
                write_to_file(lattice_groundstate[i], groundstates_folder + "lattice" + to_string(i) + ".dat");
            }
        }
        lattice_groundstate.clear();
    }


    //Generic data
    template<typename Derived>
    void store_thermo(const ArrayBase<Derived> &data, std::string name){
        write_to_file(data, thermodynamics_folder + name);

    }

    //Generic data
    template<typename T>
    void store_thermo(const T &data, std::string name){
        string folder = thermodynamics_folder;
        create_folder_master(folder,world_ID);
        write_to_file(data, thermodynamics_folder + name);
    }


    //File streams
    template<typename Derived>
    void store_samples(const ArrayBase<Derived> &data, std::string name, int store_counter){
        if (store_counter == 0){
            write_to_file(data, timeseries_folder + name);
        }else{
            append_to_file(data, timeseries_folder + name);
        }
    }

    template<typename T>
    void store_samples(vector<T> vec_data, std::string name, int store_counter){
        ArrayXd data = Map<ArrayXd>(vec_data.data(), vec_data.size());
        if (store_counter == 0){
            write_to_file(data, timeseries_folder + name);
        }else{
            append_to_file(data, timeseries_folder + name);
        }
    }


    template<typename T>
    void write_to_file(const T &data, string filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        file << data << endl;
        file.flush();
        file.close();
    }

    template<typename Derived>
    void write_to_file(const MatrixBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.flush();
        file.close();
    }

    template<typename Derived>
    void write_to_file(const ArrayBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::trunc);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.flush();
        file.close();
    }

    template<typename Derived>
    void append_to_file(const MatrixBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::app);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.flush();
        file.close();
    }

    template<typename Derived>
    void append_to_file(const ArrayBase<Derived> &data, string filename){
        ofstream file(filename,ios::out | ios::app);
        file << fixed << showpoint << setprecision(precision);
        string      _coeffSeparator = "	";
        IOFormat fmt(StreamPrecision, 0, _coeffSeparator);
        file << data.format(fmt) << endl;
        file.flush();
        file.close();
    }

};



#endif //WL_CLASS_PRINT_DATA_H
