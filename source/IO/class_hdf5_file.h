//
// Created by david on 8/1/17.
//

#ifndef DMRG_CLASS_HDF5_H
#define DMRG_CLASS_HDF5_H

#include <hdf5.h>
#include <hdf5_hl.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <mpi.h>

#include <experimental/filesystem>
#include <experimental/type_traits>

#include <type_traits>
#include <typeinfo>
#include "class_custom_cout.h"
#include <IO/nmspc_type_check.h>
#include <gitversion.h>
#include <sim_parameters/n_sim_settings.h>
#include <Eigen/Core>
#include <iterator>

namespace fs = std::experimental::filesystem::v1;
namespace tc = TypeCheck;

/*!
 \brief Writes and reads data to a binary hdf5-file.

 # HDF5 Class

*/

class class_hdf5_file {
private:
    herr_t      retval;
    fs::path    output_filename;
    fs::path    output_folder;
    fs::path    output_file_path;
    bool        create_dir;
    bool        overwrite;
    bool        mpi_io = false;

    void set_output_file_path();
    class_custom_cout ccout;


    //Mpi related constants
    int mpi_size, mpi_rank;
    hid_t plist_facc;
    hid_t plist_xfer;
    hid_t plist_lncr;


public:
    hid_t       file;

    explicit class_hdf5_file(const fs::path output_filename_, const fs::path output_dirname_ , bool create_dir_ = true, bool overwrite_ = false, bool mpi_io_=false);
    explicit class_hdf5_file(bool mpi_io_=false);

    ~class_hdf5_file(){
        H5Pclose(plist_facc);
        H5Pclose(plist_xfer);
        H5Pclose(plist_lncr);
        H5Fclose(file);
    }
    template<typename DataType>
    void extend_dataset(const DataType &data, const std::string & dataset_relative_name);
    template<typename DataType>
    void extend_dataset_mpi(const DataType &data, const std::string & dataset_relative_name);

    void create_group_link(const std::string &group_relative_name);

    template <typename AttrType>
    void create_group_link(const std::string &group_relative_name, const std::string &attribute_name, const AttrType attr);


    template <typename DataType>
    void write_dataset(const DataType &data, const std::string &dataset_relative_name);

    template <typename DataType>
    void write_dataset_mpi(const DataType &data, const std::string &dataset_relative_name);

    template <typename DataType>
    void read_dataset(DataType &data, const std::string &dataset_relative_name);

    template <typename DataType>
    void read_dataset_mpi(DataType &data, const std::string &dataset_relative_name);


    template <typename AttrType>
    void write_attribute_to_dataset(const std::string &dataset_relative_name, const AttrType attribute,
                                    const std::string &attribute_name);

    template <typename AttrType>
    void write_attribute_to_dataset_mpi(const std::string &dataset_relative_name, const AttrType attribute,
                                        const std::string &attribute_name);

    template <typename AttrType>
    void write_attribute_to_group(const std::string &group_relative_name, const AttrType attribute,
                                  const std::string &attribute_name);



private:
    void initialize();
    void initialize_mpi();



    void extend_dataset(const std::string & dataset_relative_name, const int dim, const int extent );

    void extend_dataset_mpi(const std::string & dataset_relative_name, const int dim, const int extent);

    template <typename DataType>
    void create_dataset_link(const std::string &dataset_relative_name, hsize_t *chunk_size);

    template <typename DataType>
    void create_dataset_link(const DataType &data, const std::string &dataset_relative_name);

    template <typename DataType>
    void create_dataset_link_mpi(const DataType &data, const std::string &dataset_relative_name);

    void select_hyperslab(hid_t &filespace, hid_t &memspace);

    template<typename DataType>
    hid_t get_DataType(){
        if constexpr (std::is_same<DataType, int>::value)          {return  H5Tcopy(H5T_NATIVE_INT);}
        if constexpr (std::is_same<DataType, long>::value)         {return  H5Tcopy(H5T_NATIVE_LONG);}
        if constexpr (std::is_same<DataType, double>::value)       {return  H5Tcopy(H5T_NATIVE_DOUBLE);}
        if constexpr (std::is_same<DataType, float>::value)        {return  H5Tcopy(H5T_NATIVE_FLOAT);}
        if constexpr (std::is_same<DataType, char>::value)         {return  H5Tcopy(H5T_C_S1);}
        if constexpr (std::is_same<DataType, std::string>::value)  {return  H5Tcopy(H5T_C_S1);}
        if constexpr (tc::has_member_scalar <DataType>::value)   {return  get_DataType<typename DataType::Scalar>();}
        if constexpr (tc::has_member_value_type<DataType>::value){return  get_DataType<typename DataType::value_type>();}
        std::cerr << "get_DataType could not match the type provided" << std::endl;
        exit(1);
    }



    template<typename DataType>
    int get_Size(const DataType &data){

        if constexpr (tc::is_tensor<DataType>::value){
            return (int) data.size();
        }

        if constexpr (std::is_arithmetic<DataType>::value){
            return 1;
        }
        if constexpr (tc::is_matrix <DataType>::value) {
            return data.size();
        }
        if constexpr(tc::is_instance_of<std::vector,DataType>::value){
            return data.size();
        }

        if constexpr(std::is_same<std::string, DataType>::value){
            return data.size();
        }
        std::cerr << "get_Rank can't match the type provided: " << typeid(data).name() << '\n';
        exit(1);
    }

    template<typename DataType>
    constexpr int get_Rank() const{

//        if constexpr (tc::is_tensor<DataType>::value){
//            return (int) data.rank();
//        }

        if constexpr (std::is_arithmetic<DataType>::value){
            return 1;
        }
        if constexpr (tc::is_matrix <DataType>::value) {
            return 2;
        }
        if constexpr(tc::is_instance_of<std::vector,DataType>::value){
            return 1;
        }

        if constexpr(std::is_same<std::string, DataType>::value){
            return 1;
        }
        std::cerr << "get_Rank can't match the type provided: " << typeid(DataType).name() << '\n';
        exit(1);
    }


    template<typename DataType>
    hid_t get_DataSpace_unlimited() const {

//        if constexpr (tc::is_tensor<DataType>::value){
//            int rank = data.rank();
//            hsize_t dims[rank];
////            hsize_t max_dims[rank] = {H5S_UNLIMITED};
//            std::copy(data.dimensions().begin(), data.dimensions().end(), dims);
//            return H5Screate_simple(rank, dims, nullptr);
//        }
        if constexpr (std::is_arithmetic<DataType>::value){
            const int rank = 1;
            hsize_t dims[rank] = {0};
            hsize_t max_dims[rank] = {H5S_UNLIMITED};
            return H5Screate_simple(rank, dims, max_dims);
        }
        if constexpr (tc::is_matrix <DataType>::value) {
            const int rank = 2;
            hsize_t dims[rank]      = {0, 0};
            hsize_t max_dims[rank]  = { H5S_UNLIMITED, H5S_UNLIMITED };
            return H5Screate_simple(rank, dims, max_dims);
        }
        if constexpr(tc::is_instance_of<std::vector,DataType>::value){
            const int rank = 1;
            hsize_t dims[rank] = {0};
            hsize_t max_dims[rank] = {H5S_UNLIMITED};
            return H5Screate_simple(rank, dims, max_dims);
        }

        if constexpr(std::is_same<std::string, DataType>::value){
            const int rank = 1;
            hsize_t dims[rank] = {0};
            hsize_t max_dims[rank] = {H5S_UNLIMITED};
            return H5Screate_simple(rank,dims, max_dims);
        }
        std::cerr << "get_DataSpace_unlimited can't match the type provided: " << typeid(DataType).name() << '\n';
        exit(1);
    }


    template<typename DataType>
    hid_t get_MemSpace(const DataType &data){

//        if constexpr (tc::is_tensor<DataType>::value){
//            int rank = get_Rank<DataType>();
//            hsize_t dims[rank];
//            std::copy(data.dimensions().begin(), data.dimensions().end(), dims);
//            return H5Screate_simple(rank, dims, nullptr);
//        }
        if constexpr (std::is_arithmetic<DataType>::value){
            constexpr int rank = 1;
            hsize_t dims[rank] = {1};
            return H5Screate_simple(rank, dims, nullptr);
        }
        if constexpr (tc::is_matrix <DataType>::value) {
            constexpr int rank = 2;
            hsize_t dims[rank] = { (hsize_t) data.rows(), (hsize_t) data.cols() };
            return H5Screate_simple(rank, dims, nullptr);
        }
        if constexpr(tc::is_instance_of<std::vector,DataType>::value){
            constexpr int rank = 1;
            hsize_t dims[rank] = {data.size()};
            return H5Screate_simple(rank, dims, NULL);
        }

        if constexpr(std::is_same<std::string, DataType>::value){
            constexpr int rank = 1;
            hsize_t dims[rank] = {1};
            return H5Screate_simple(rank,dims, nullptr);
        }
        std::cerr << "get_MemSpace can't match the type provided: " << typeid(DataType).name() << '\n';
        exit(1);
    }


    template <typename DataType>
    void set_ChunkDims(const DataType &data, hsize_t* dims){
//        if constexpr (tc::is_tensor<DataType>::value){
//            std::copy(data.dimensions().begin(), data.dimensions().end(), dims);
//            return;
//        }
        if constexpr (std::is_arithmetic<DataType>::value){
            dims[0]={100};
            return;
        }
        if constexpr (tc::is_matrix <DataType>::value) {
            dims[0] = (hsize_t) data.rows();
            dims[1] = (hsize_t) data.cols();
            return;
        }
        if constexpr(tc::is_instance_of<std::vector,DataType>::value){
            dims[0]={data.size()};
            return;
        }

        if constexpr(std::is_same<std::string, DataType>::value){
            dims[0]={data.size()};
            return;
        }
        std::cerr << "get_ChunkDims can't match the type provided: " << typeid(DataType).name() << '\n';
        exit(1);
    }


};



#endif
