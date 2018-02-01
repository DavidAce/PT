//
// Created by david on 8/17/17.
//

#ifndef DMRG_N_DATA_CONTAINERS_H
#define DMRG_N_DATA_CONTAINERS_H
#include <vector>
#include <IO/class_hdf5_dataset_buffer.h>
#include <sim_parameters/n_model.h>
//#include "mps_routines/class_measurement.h"
enum class Observable {energy, entropy, variance, error, time};
enum class Meta_data  {chi, chi_max, chain_length, iteration, phys_time, wall_time };

class class_measurement_buffers{
private:
    class_hdf5 *hdf5_out = nullptr; //Pointer is not owned! do not delete
    bool data_has_been_written_to_file = false;
public:
    class_hdf5_dataset_buffer<long>   chi;
    class_hdf5_dataset_buffer<long>   chi_max;
    class_hdf5_dataset_buffer<double> energy;
    class_hdf5_dataset_buffer<double> energy_ex; //Exact energy
    class_hdf5_dataset_buffer<double> entropy;
    class_hdf5_dataset_buffer<double> variance1;
    class_hdf5_dataset_buffer<double> variance2;
    class_hdf5_dataset_buffer<double> trerror;
    class_hdf5_dataset_buffer<double> e_error;
    class_hdf5_dataset_buffer<int>    iter_step;
    class_hdf5_dataset_buffer<double> time_step;
    class_hdf5_dataset_buffer<double> phys_time;
    class_hdf5_dataset_buffer<double> wall_time;
    class_hdf5_dataset_buffer<long>   length;


    class_measurement_buffers() = default;


    class_measurement_buffers(const std::string &group_name,
                              const int iteration):
            class_measurement_buffers(nullptr,group_name, iteration)
    {}


    class_measurement_buffers(class_hdf5 * hdf5_out_ ,
                            const std::string &group_name,
                            const int iteration):
    class_measurement_buffers(hdf5_out_, group_name, iteration, 0, "default_attribute")
    {}

    template<typename IterType, typename AttrType>
    class_measurement_buffers(const std::string &group_name,
                              const IterType iteration,
                              const AttrType &attribute,
                              const std::string &attribute_name):
            class_measurement_buffers(nullptr, group_name, iteration, attribute, attribute_name)
    {}

    template<typename IterType, typename AttrType>
    class_measurement_buffers(class_hdf5 * hdf5_out_ ,
                              const std::string &group_name,
                              const IterType iteration,
                              const AttrType &attribute,
                              const std::string &attribute_name):
            hdf5_out   (hdf5_out_),
            chi        (group_name, iteration, "chi"             , attribute, attribute_name ),
            chi_max    (group_name, iteration, "chi_max"         , attribute, attribute_name ),
            energy     (group_name, iteration, "energy"          , attribute, attribute_name ),
            energy_ex  (group_name, iteration, "energy_ex"       , attribute, attribute_name ),
            entropy    (group_name, iteration, "entropy"         , attribute, attribute_name ),
            variance1  (group_name, iteration, "variance1"       , attribute, attribute_name ),
            variance2  (group_name, iteration, "variance2"       , attribute, attribute_name ),
            trerror    (group_name, iteration, "truncation_error", attribute, attribute_name ),
            e_error    (group_name, iteration, "energy_error"    , attribute, attribute_name ),
            iter_step  (group_name, iteration, "iter_step"       , attribute, attribute_name ),
            time_step  (group_name, iteration, "time_step"       , attribute, attribute_name ),
            phys_time  (group_name, iteration, "phys_time"       , attribute, attribute_name ),
            wall_time  (group_name, iteration, "wall_time"       , attribute, attribute_name ),
            length     (group_name, iteration, "length"          , attribute, attribute_name )
    {}


    ~class_measurement_buffers(){
        if (hdf5_out != nullptr){
            write_data(*hdf5_out);
        }else
        if (!data_has_been_written_to_file){
            std::cerr << "Output data has not saved to file, yet it is being discarded!\n" << std::endl;
        }
    }

    class_measurement_buffers(class_measurement_buffers&& other)
    : class_measurement_buffers() // initialize via default constructor, C++11 only
    {
        std::swap(*this, other);
    }
    class_measurement_buffers& operator=(class_measurement_buffers rhs)
    {
        std::swap(rhs);
        return *this;
    }


    void set(class_hdf5 * hdf5_out_ ,
             const std::string &group_name,
             const int iteration){
        *this = class_measurement_buffers(hdf5_out_, group_name, iteration);
    }

    void set(const std::string &group_name,
             const int iteration){
        *this = class_measurement_buffers(group_name, iteration);
    }

    template<typename AttrType>
    void set(class_hdf5 *hdf5_out_,
             const std::string &group_name,
             const int &iteration,
             const AttrType &attribute,
             const std::string &attribute_name){
        *this = class_measurement_buffers(group_name, iteration, attribute, attribute_name);
    }





//    void push_back(class_measurement &measurement){
//        chi      .push_back(measurement.get_chi());
//        chi_max  .push_back(measurement.get_chi_max());
//        energy   .push_back(measurement.get_energy());
//        energy_ex.push_back(class_model::energy_exact);
//        entropy  .push_back (measurement.get_entropy());
//        variance1 .push_back(measurement.get_variance1());
//        variance2 .push_back(measurement.get_variance2());
//        trerror  .push_back (measurement.get_truncation_error());
//        length   .push_back (measurement.get_chain_length());
//    }

//
//    void push_back(int    iter_step_,
//                   double time_step_,
//                   double phys_time_,
//                   double wall_time_)
//    {
//        iter_step.push_back(iter_step_);
//        time_step.push_back(time_step_);
//        phys_time.push_back(phys_time_);
//        wall_time.push_back(wall_time_);
//    }
//
//    void push_back(long chi_cur_,
//                   long chi_max_,
//                   double energy_,
//                   double entropy_,
//                   double variance1_,
//                   double variance2_,
//                   double variance3_,
//                   double trunc_error_,
//                   double time_step_,
//                   double phys_time_,
//                   double wall_time_)
//    {
//        chi      .push_back(chi_cur_);
//        chi_max  .push_back(chi_max_);
//        energy   .push_back(energy_);
//        entropy  .push_back(entropy_);
//        variance1 .push_back(variance1_);
//        variance2 .push_back(variance2_);
//        trerror  .push_back(trunc_error_);
//        time_step.push_back(time_step_);
//        phys_time.push_back(phys_time_);
//        wall_time.push_back(wall_time_);
//    }

    void write_data(class_hdf5 & hdf5_out){
        chi      .write_buffer_to_file(hdf5_out);
        chi_max  .write_buffer_to_file(hdf5_out);
        energy   .write_buffer_to_file(hdf5_out);
        energy_ex.write_buffer_to_file(hdf5_out);
        entropy  .write_buffer_to_file(hdf5_out);
        variance1 .write_buffer_to_file(hdf5_out);
        variance2 .write_buffer_to_file(hdf5_out);
        trerror  .write_buffer_to_file(hdf5_out);
        e_error  .write_buffer_to_file(hdf5_out);
        iter_step.write_buffer_to_file(hdf5_out);
        time_step.write_buffer_to_file(hdf5_out);
        phys_time.write_buffer_to_file(hdf5_out);
        wall_time.write_buffer_to_file(hdf5_out);
        length   .write_buffer_to_file(hdf5_out);
        data_has_been_written_to_file = true;
    }
};


#endif //DMRG_N_DATA_CONTAINERS_H
