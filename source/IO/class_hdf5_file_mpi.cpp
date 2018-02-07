//
// Created by david on 2018-02-07.
//

#include <IO/class_hdf5_file.h>



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


void class_hdf5_file::extend_dataset_mpi(const std::string & dataset_relative_name, int dim, int extent ){
    std::vector<int> all_extents((unsigned long)mpi_size);
    MPI_Allgather(&extent, 1, MPI_INT, all_extents.data(), 1, MPI_INT, MPI_COMM_WORLD);
    for (int i = 0; i < mpi_size; i++) {
        std::string dataset_relative_name_n = dataset_relative_name + std::to_string(i);
        extend_dataset(dataset_relative_name_n, dim, all_extents[i]);
    }

}
