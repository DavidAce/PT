//
// Created by david on 2018-02-05.
//


#include "class_hdf5_file.h"
#include <H5Cpp.h>
#include <fstream>
#include <istream>
#include <directory.h>
#include <thread>
#include <chrono>
#include <mpi.h>


template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    if ( !v.empty() ) {
        out << "[ ";
        std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, " "));
        out << "]";
    }
    return out;
}


void class_hdf5_file::initialize_mpi(){
/*
* Initialize MPI
*/
    set_output_file_path();

    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Info info = MPI_INFO_NULL;

    MPI_Comm_size(comm, &mpi_size);
    MPI_Comm_rank(comm, &mpi_rank);


    // Set up file access property list with parallel I/O access


    plist_facc = H5Pcreate(H5P_FILE_ACCESS);
    plist_xfer = H5Pcreate(H5P_DATASET_XFER);
    plist_lncr = H5Pcreate(H5P_LINK_CREATE);   //Create missing intermediate group if they don't exist
    H5Pset_fapl_mpio(plist_facc, comm, info);
    H5Pset_dxpl_mpio(plist_xfer, H5FD_mpio_xfer_t::H5FD_MPIO_INDEPENDENT);
//    H5Pset_create_intermediate_group(plist_lncr, 1);

    // Create a new file collectively and release property list identifier.
    file = H5Fcreate(output_file_path.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, plist_facc);


    //    Put git revision in file attribute
    std::string gitversion = "Branch: " + GIT::BRANCH + " | Commit hash: " + GIT::COMMIT_HASH + " | Revision: " + GIT::REVISION;
    hid_t datatype          = get_DataType<std::string>();
    hid_t dataspace         = get_MemSpace(gitversion);
    retval                  = H5Tset_size(datatype, gitversion.size());
    retval                  = H5Tset_strpad(datatype,H5T_STR_NULLTERM);
    hid_t attribute         = H5Acreate(file, "GIT REVISION", datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT );
    retval                  = H5Awrite(attribute, datatype, gitversion.c_str());
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Aclose(attribute);

}

//void class_hdf5_file::create_group_mpi(const std::string & group_relative_name) {
//    //Check if group exists already
//    if (!H5Lexists(file, group_relative_name.c_str(), H5P_DEFAULT)) {
//        hid_t group = H5Gcreate(file, group_relative_name.c_str(), plist_lncr, H5P_DEFAULT, H5P_DEFAULT);
//        H5Gclose(group);
//    }
//}

template <typename DataType>
void class_hdf5_file::create_dataset_link_mpi(const DataType &data, const std::string &dataset_relative_name){

    for (int i = 0; i < mpi_size; i++) {
        std::string dataset_relative_name_n = dataset_relative_name + std::to_string(i);
        if (!H5Lexists(file, dataset_relative_name_n.c_str(), H5P_DEFAULT)) {
            auto rank = get_Rank<DataType>();
            hsize_t chunk_dims[rank];
            set_ChunkDims(data,chunk_dims);
            create_dataset_link<DataType>(dataset_relative_name_n, chunk_dims);
        }
    }
}


template<typename DataType>
void class_hdf5_file::extend_dataset_mpi(const DataType &data, const std::string & dataset_relative_name){
    if constexpr (tc::is_matrix<DataType>::value){
        extend_dataset_mpi(dataset_relative_name, 0, data.rows());
        std::string dataset_relative_name_n = dataset_relative_name + std::to_string(0);
        hid_t dataset   = H5Dopen(file, dataset_relative_name_n.c_str(), H5P_DEFAULT);
        hid_t filespace = H5Dget_space(dataset);
        int ndims = H5Sget_simple_extent_ndims(filespace);
        hsize_t dims[ndims];
        H5Sget_simple_extent_dims(filespace,dims,NULL);
        H5Dclose(dataset);
        H5Sclose(filespace);
        if (dims[1] < data.cols()){
            extend_dataset_mpi(dataset_relative_name, 1, data.cols());
        }
    }
    else{
        extend_dataset_mpi(dataset_relative_name, 0, get_Size(data));
    }
}


void class_hdf5_file::extend_dataset_mpi(const std::string & dataset_relative_name, const int dim, const int extent ){
    std::vector<int> all_extents((ulong)mpi_size);
    MPI_Allgather(&extent, 1, MPI_INT, all_extents.data(), 1, MPI_INT, MPI_COMM_WORLD);
    for (int i = 0; i < mpi_size; i++) {
        std::string dataset_relative_name_n = dataset_relative_name + std::to_string(i);
        extend_dataset(dataset_relative_name_n, dim, all_extents[i]);
    }

}



template <typename DataType>
void class_hdf5_file::write_dataset_mpi(const DataType &data, const std::string &dataset_relative_name){
    if (mpi_io){
        create_dataset_link_mpi(data, dataset_relative_name);
        extend_dataset_mpi(data,dataset_relative_name);
        std::string dataset_relative_name_n = dataset_relative_name + std::to_string(mpi_rank);
        hid_t dataset   = H5Dopen(file, dataset_relative_name_n.c_str(), H5P_DEFAULT);
        hid_t datatype  = get_DataType<DataType>();
        hid_t filespace = H5Dget_space(dataset);
        hid_t memspace  = get_MemSpace(data);
        select_hyperslab(filespace,memspace);

        if constexpr(std::is_arithmetic<DataType>::value){
            retval = H5Dwrite(dataset, datatype, memspace, filespace, plist_xfer, &data);
        }

        if constexpr(tc::has_member_data<DataType>::value){
            retval = H5Dwrite(dataset, datatype, memspace, filespace, plist_xfer, data.template data());
        }
        if (retval < 0){
            std::cerr << "Could not write to file!!!" <<std::endl;
        }
        MPI_Barrier(MPI_COMM_WORLD);

        H5Tclose(datatype);
        H5Dclose(dataset);
        H5Sclose(filespace);
        H5Sclose(memspace);

    }else{
        std::cerr << "mpi_io is set to false, please initialize hdf5 object with mpi_io=true" << std::endl;
        exit(1);
    }
}


//Explicit instantiations
template void class_hdf5_file::write_dataset_mpi(const int &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const long &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const double &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const float &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const char &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const std::string &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const std::vector<int> &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const std::vector<long> &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const std::vector<double> &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const Eigen::ArrayXi &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const Eigen::ArrayXd &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const Eigen::ArrayXXi &, const std::string &);
template void class_hdf5_file::write_dataset_mpi(const Eigen::ArrayXXd &, const std::string &);


template <typename DataType>
void class_hdf5_file::read_dataset_mpi(DataType &data, const std::string &dataset_relative_name){
    if(mpi_io) {
        for (int i = 0; i < mpi_size; i++){
            if(mpi_rank == i) {
                std::string dataset_relative_name_n = dataset_relative_name + std::to_string(i);
                read_dataset(data, dataset_relative_name_n);
            }
        }

    }else{
        std::cerr << "mpi_io is set to false, please initialize hdf5 object with mpi_io=true" << std::endl;
        exit(1);
    }
}
template void class_hdf5_file::read_dataset_mpi( std::vector<int> &data, const std::string &dataset_relative_name);
template void class_hdf5_file::read_dataset_mpi( std::vector<double> &data, const std::string &dataset_relative_name);
template void class_hdf5_file::read_dataset_mpi( Eigen::ArrayXi &data, const std::string &dataset_relative_name);
template void class_hdf5_file::read_dataset_mpi( Eigen::ArrayXd &data, const std::string &dataset_relative_name);


template <typename AttrType>
void class_hdf5_file::write_attribute_to_dataset_mpi(const std::string &dataset_relative_name, const AttrType attribute,
                                                     const std::string &attribute_name){
    if(mpi_io) {
        for (int i = 0; i < mpi_size; i++){
            std::string dataset_relative_name_n = dataset_relative_name + std::to_string(i);
            write_attribute_to_dataset(dataset_relative_name_n, attribute,attribute_name);

        }
    }else{
        std::cerr << "mpi_io is set to false, please initialize hdf5 object with mpi_io=true" << std::endl;
        exit(1);
    }
}

//Explicit instantiations
template void class_hdf5_file::write_attribute_to_dataset_mpi(const std::string & ,  const int  ,const std::string & );
template void class_hdf5_file::write_attribute_to_dataset_mpi(const std::string & ,  const long  ,const std::string & );
template void class_hdf5_file::write_attribute_to_dataset_mpi(const std::string & ,  const double  ,const std::string & );
