//
// Created by david on 2018-01-14.
//

#include "n_sim_settings.h"
#include <IO/class_file_reader.h>
using namespace std;
namespace settings{

    double sim::T_min                      = 0.1 ;                      /*!< Minimum temperature of the simulation */
    double sim::T_max                      = 6   ;                      /*!< Maximum temperature of the simulation */
    int    sim::MCS_warmup                 = 10000 ;                    /*! Number of warmup Monte Carlo Sweeps (MCS) */
    int    sim::MCS_sample                 = 20000 ;                     /*! Number of sampling Monte Carlo Sweeps (MCS) */
    string sim::job_name                   = "";                        /*!< Name or identifier for the current job   */

    bool   model::on                       = true;                      /*!< Turns Ising simulation on/off. */
    double model::J1                       = 1   ;                      /*!< Nearest neighbor (Ferromagnetic) coupling */
    double model::J2                       = 0   ;                      /*!< (NOT IMPLEMENTED) Next nearest neighbor coupling */
    double model::J3                       = 0   ;                      /*!< (NOT IMPLEMENTED) Third nearest neighbor coupling */
    int    model::L                        = 12  ;                      /*!< Linear lattice size */
    int    model::N                        = model::L*model::L;         /*!< Number of lattice points */
    string model::model_name               = "";                        /*!< Name or identifier for the current model */

//    bool   model_j1j2::on                 = true;                      /*!< Turns J1J2 simulation on/off. */
//    double model_j1j2::J1                 = 1   ;                      /*!< Nearest neighbor coupling */
//    double model_j1j2::J2                 = 1   ;                      /*!< Next Nearest neighbor coupling */

    //Rates for checking, sample, storing and printing (MCS units)
    int rate::prob                        = 1;                         /*!< How often to sample */
    int rate::sort                        = 10000;                     /*!< How often to sort all samplings and write timeseries data to file */
    int rate::comp                        = 50000;                     /*!< How often to compute statistics for printing mid-simulation */
    int rate::swap                        = 40;                        /*!< How often to swap walkers in adjacent windows */
    int rate::cout                        = 10000;                     /*!< How often to cout in terminal */
    int rate::move                        = 5000;                      /*!< How often to move temperatures during warmup */
    int rate::sync                        = 100;                       /*!< How often to sync groundstates during warmup (to find the lowest energy states) */

    //Number of calc_thermo cycles for error calculation
    int bootstrap::reps                   = 100;                      /*!< Number of calc_thermo cycles for error calculation */

    //Save data to hdf5
    bool   hdf5::save_to_file             = true;
    bool   hdf5::create_dir_if_not_found  = true;
    bool   hdf5::overwrite_file_if_found  = false;
    string hdf5::output_filename          = "data.h5";
    string hdf5::output_folder            = "output";
    bool   hdf5::full_storage             = true;

    //Profiling
    bool profiling::on                   = false;
    int  profiling::precision            = 5;
    //Console settings
    int  console::verbosity              = 0;
    bool console::timestamp              = false;

}



void settings::initialize(class_file_reader &indata){
    sim::T_min                      = indata.find_parameter<double> ("sim::T_min"     ,    sim::T_min    );
    sim::T_max                      = indata.find_parameter<double> ("sim::T_max"     ,    sim::T_max    );
    sim::MCS_warmup                 = indata.find_parameter<int>    ("sim::MCS_warmup",    sim::MCS_warmup   );
    sim::MCS_sample                 = indata.find_parameter<int>    ("sim::MCS_sample",    sim::MCS_sample   );
    sim::job_name                   = indata.find_parameter<string> ("sim::job_name"  ,    sim::job_name  );

    model::on                       = indata.find_parameter<bool>   ("model::on"        ,    model::on   );
    model::J1                       = indata.find_parameter<double> ("model::J1"        ,    model::J1    );
    model::J2                       = indata.find_parameter<double> ("model::J2"        ,    model::J2    );
    model::J3                       = indata.find_parameter<double> ("model::J3"        ,    model::J3    );
    model::L                        = indata.find_parameter<int>    ("model::L"           ,    model::L        );
    model::N                        = model::L * model::L;
    model::model_name               = indata.find_parameter<string> ("model::model_name"  ,    model::model_name);

//    model_j1j2::on                  = indata.find_parameter<bool>   ("model_j1j2::on"        ,    model_j1j2::on       );
//    model_j1j2::J1                  = indata.find_parameter<double> ("model_j1j2::J1"        ,    model_j1j2::J1       );
//    model_j1j2::J2                  = indata.find_parameter<double> ("model_j1j2::J2"        ,    model_j1j2::J2       );


    //Rates for checking, sample, storing and printing (MCS units)
    rate::prob                      = indata.find_parameter<int> ("rate::prob",rate::prob);
    rate::sort                      = indata.find_parameter<int> ("rate::sort",rate::sort);
    rate::comp                      = indata.find_parameter<int> ("rate::comp",rate::comp);
    rate::swap                      = indata.find_parameter<int> ("rate::swap",rate::swap);
    rate::cout                      = indata.find_parameter<int> ("rate::cout",rate::cout);
    rate::move                      = indata.find_parameter<int> ("rate::move",rate::move);
    rate::sync                      = indata.find_parameter<int> ("rate::sync",rate::sync);

    bootstrap::reps                 = indata.find_parameter<int> ("bootstrap::reps",bootstrap::reps);


    //Save data to hdf5
    hdf5::save_to_file             = indata.find_parameter<bool>   ("hdf5::save_to_file"            , hdf5::save_to_file           );
    hdf5::create_dir_if_not_found  = indata.find_parameter<bool>   ("hdf5::create_dir_if_not_found" , hdf5::create_dir_if_not_found);
    hdf5::overwrite_file_if_found  = indata.find_parameter<bool>   ("hdf5::overwrite_file_if_found" , hdf5::overwrite_file_if_found);
    hdf5::output_filename          = indata.find_parameter<string> ("hdf5::output_filename"         , hdf5::output_filename);
    hdf5::output_folder            = indata.find_parameter<string> ("hdf5::output_folder"           , hdf5::output_folder);
    hdf5::full_storage             = indata.find_parameter<bool>   ("hdf5::full_storage"            , hdf5::full_storage           );

    //Profiling
    profiling::on                  = indata.find_parameter<bool>   ("profiling::on"        , profiling::on        );
    profiling::precision           = indata.find_parameter<int>    ("profiling::precision" , profiling::precision );
    //Console settings
    console::verbosity             = indata.find_parameter<int>    ("console::verbosity"   , console::verbosity);
    console::timestamp             = indata.find_parameter<bool>   ("console::timestamp"   , console::timestamp);
}
