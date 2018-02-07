//
// Created by david on 8/7/17.
//

#ifndef PT_N_SETTINGS_H
#define PT_N_SETTINGS_H
#include <string>
/*! \brief General settings.*/

/*!
 *  \namespace settings
 *  This namespace contains settings for the simulation
 *
 */

class class_file_reader;

namespace settings {
    extern void initialize(class_file_reader &indata);

    namespace sim{
        extern double       T_min     ;                          /*!< Minimum temperature of the simulation */
        extern double       T_max     ;                          /*!< Maximum temperature of the simulation */
        extern int          MCS_warmup;                          /*! Number of warmup Monte Carlo Sweeps (MCS) */
        extern int          MCS_sample;                          /*! Number of sampling Monte Carlo Sweeps (MCS) */
        extern std::string  job_name  ;                          /*! Name or identifier for the current job   */
    }

    namespace model{
        extern bool         on        ;                          /*!< Turns simulation on/off. */
        extern double       J1        ;                          /*!< Nearest neighbor (Ferromagnetic) coupling */
        extern double       J2        ;                          /*!< (NOT IMPLEMENTED) Next nearest neighbor coupling */
        extern double       J3        ;                          /*!< (NOT IMPLEMENTED) Third nearest neighbor coupling */
        extern int          L         ;                          /*!< Linear lattice size */
        extern int          N         ;                          /*!< Number of lattice points */
        extern std::string  model_name;                          /*!< Name or identifier for the current model */
    }

//    namespace model_j1j2 {
//        extern bool         on        ;                          /*!< Turns J1J2 simulation on/off. */
//        extern double       J1        ;                          /*! Coupling 1 */
//        extern double       J2        ;                          /*! Coupling 2 */
//    }


    namespace rate {
        //Rates for checking, sample, storing and printing (MCS units)
        extern int samp;                                    /*!< How often to sample */
        extern int save;                                    /*!< How often to save all samplings and write timeseries to file */
        extern int comp;                                    /*!< How often to compute full statistics from entire sampling period */
        extern int swap;                                    /*!< How often to swap walkers in adjacent windows */
        extern int cout;                                    /*!< How often to cout in terminal */
        extern int move;                                    /*!< How often to move temperatures during warmup */
        extern int sync;                                    /*!< How often to sync groundstates during warmup (to find the lowest energy states) */

    }

    namespace bootstrap{
        //Number of calc_thermo cycles for error calculation
        extern int reps;
    }

    //Save data to hdf5 (NOT FULLY IMPLEMENTED YET)
    namespace hdf5 {
        extern bool         save_to_file            ;        /*!< If true, saves the simulation data to an HDF5 file instead of just outputting to console */
        extern bool         create_dir_if_not_found ;        /*!< If true, an output directory will be created in the project root folder if it isn't found */
        extern bool         overwrite_file_if_found ;        /*!< If true, an hdf5-file with the provided filename will be overwritten if found in output_folder */
        extern std::string  output_filename         ;        /*!< Name of the output HDF5 file */
        extern std::string  output_folder           ;        /*!< Path of the output HDF5 file */
        extern bool         full_storage            ;        /*!< If true, saves more simulation data to file. Set to false to reduce output file size. */
    }
    //Profiling
    namespace profiling {
        extern bool     on        ;                  /*!< If true, turns on profiling and timings will be shown on console. */
        extern int      precision ;                  /*!< Sets precision (number of decimals) of time output. */
    }
    //Console settings
    namespace console {
        extern int  verbosity ;                      /*!< Level of verbosity desired [0-2]. Level 0 prints almost nothing, level 2 prints everything */
        extern bool timestamp ;                      /*!< Whether to put a timestamp on console outputs */
    }
};
#endif // PT_N_SETTINGS_H
