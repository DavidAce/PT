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
#include <IO/class_hdf5_thermo_table.h>
#include <IO/class_hdf5_model_table.h>


using namespace std;
using namespace Eigen;


class output {
private:

    const string      timeseries_group     = "timeseries";
    const string      groundstates_group   = "groundstates";
    const string      thermodynamics_group = "thermodynamics";
    const string      model_group = "model";

public:
//    std::shared_ptr<class_worker> worker;
    std::shared_ptr<class_hdf5_file> hdf5;
    std::shared_ptr<class_hdf5_thermo_table> thermo_table;
    std::shared_ptr<class_hdf5_model_table> model_table;
    bool thermodynamics_has_data = false;
    bool timeseries_has_data = false;
    explicit output();

    //Groundstate Lattices
    void store_groundstates(std::vector<ArrayXXi> &lattice_groundstates, std::vector<double> & energy_groundstates){
        assert(lattice_groundstates.size() == energy_groundstates.size() and "Size mismatch on collected ground states!");
        for (int i = 0; i < (int)lattice_groundstates.size(); i++){
            hdf5->write_dataset_mpi(lattice_groundstates[i], groundstates_group + "/" + "lattice" + std::to_string(i), false);
            hdf5->write_attribute_to_dataset(groundstates_group + "/" + "lattice" + std::to_string(i),energy_groundstates[i], "energy");
        }
        lattice_groundstates.clear();
        energy_groundstates.clear();
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
    void store_model(){
        model_table->emplace_back(settings::model::J1,
                                  settings::model::J2,
                                  settings::model::J3,
                                  settings::model::L);
    }


    //Anything
    template<typename T>
    void store_generic(const T &data,std::string group,  std::string name, bool each_mpi_thread = true){
        hdf5->create_group_link(group);
        hdf5->write_dataset_mpi(data,  group +"/"+ name, each_mpi_thread);
    }
};



#endif //WL_CLASS_PRINT_DATA_H
