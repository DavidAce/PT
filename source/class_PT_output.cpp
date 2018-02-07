//
// Created by david on 2016-07-26.
//
#include <iomanip>
#include "class_PT_output.h"


output::output()
        :
        hdf5(std::make_shared<class_hdf5_file>(true)),
        thermo_table(std::make_shared<class_hdf5_thermo_table>(hdf5,thermodynamics_group,"T", true)),
        model_table(std::make_shared<class_hdf5_model_table>(hdf5,model_group,"model", false))
{
    //Create folders
    hdf5->create_group_link(timeseries_group);
    hdf5->create_group_link(groundstates_group);
    hdf5->create_group_link(thermodynamics_group);
    hdf5->create_group_link(model_group);
};
