//
// Created by david on 2018-02-07.
//


#include <IO/class_hdf5_model_table.h>
#include <IO/class_hdf5_file.h>
#include <mpi.h>


void class_hdf5_model_table::initialize_table_mpi(){
    if (this->empty() and !table_is_ready) {
        hsize_t NRECORDS = this->size();
        hdf5_out->create_group_link(group_name);
        MPI_Comm comm = MPI_COMM_WORLD;
        MPI_Comm_size(comm, &mpi_size);
        MPI_Comm_rank(comm, &mpi_rank);

        for (int i = 0; i < mpi_size; i++) {
            std::string table_path_n = table_path + std::to_string(i);
            std::string table_name_n = table_name + std::to_string(i);
            H5TBmake_table(table_name_n.c_str(), hdf5_out->file, table_path_n.c_str(), meta.NFIELDS, NRECORDS,
                           meta.dst_size, meta.field_names, meta.dst_offset, meta.field_type,
                           meta.chunk_size, meta.fill_data, meta.compress, this->data());
        }
        table_is_ready = true;
    }else{
        std::cerr << "Table not empty!!" << std::endl;
    }

}


void class_hdf5_model_table::write_buffer_to_file_mpi() {
    if (!this->empty() and table_is_ready) {
        //This only works if there is exactly the same amount of data in each record!
        hsize_t NRECORDS = this->size();
        for (int i = 0; i < mpi_size; i++) {
            std::string table_path_n = table_path + std::to_string(i);
            H5TBappend_records(hdf5_out->file, table_path_n.c_str(), NRECORDS, meta.dst_size, meta.dst_offset,
                               meta.dst_sizes, this->data());
        }
        std::string table_path_n = table_path + std::to_string(mpi_rank);
        H5TBwrite_records(hdf5_out->file, table_path_n.c_str(),recorded_elements, NRECORDS, meta.dst_size, meta.dst_offset,
                          meta.dst_sizes, this->data());
        recorded_elements += NRECORDS;
        this->clear();
        buffer_is_empty = true;
    }else{
        std::cerr << "Table is empty!!" << std::endl;
    }
}