//
// Created by david on 2018-01-26.
//

#include <IO/class_hdf5_table_buffer.h>
#include <IO/class_hdf5_file.h>
#include <H5Cpp.h>
#include <hdf5_hl.h>



class_hdf5_table_buffer::class_hdf5_table_buffer(std::shared_ptr<class_hdf5_file> hdf5_out_,
                                                 const std::string group_name_,
                                                 const std::string table_name_,
                                                 const bool mpi_on_ = false   )
        :
        hdf5_out(std::move(hdf5_out_)),
        group_name(group_name_),
        table_name(table_name_),
        mpi_on(mpi_on_)
{
    table_path = group_name + "/" + table_name;
//    this->reserve(max_elements);
    if(mpi_on){
        std::cout << "Initializing table "<< std::endl;
        initialize_table_mpi();

    }else{
        initialize_table();
    }

};

class_hdf5_table_buffer::class_hdf5_table_buffer(std::nullptr_t nullp,
                                                 const std::string group_name_,
                                                 const std::string table_name_,
                                                 const bool mpi_on_ = false   )
        :
        group_name(group_name_),
        table_name(table_name_),
        mpi_on(mpi_on_)
{
    table_path = group_name + "/" + table_name;
//    this->reserve(max_elements);
    if(mpi_on){
        initialize_table_mpi();

    }else{
        initialize_table();
    }
};


class_hdf5_table_buffer::class_hdf5_table_buffer(const std::string group_name_,
                                                 const std::string table_name_,
                                                 const bool mpi_on_ = false  )
        : class_hdf5_table_buffer(nullptr, group_name_,table_name_, mpi_on_)
{

}

void class_hdf5_table_buffer::initialize_table(){
    if (this->empty() and !table_is_ready) {

        hsize_t NRECORDS = this->size();
        hdf5_out->create_group_link(group_name);
        H5TBmake_table(table_name.c_str(), hdf5_out->file, table_path.c_str(), meta.NFIELDS, NRECORDS,
                       meta.dst_size, meta.field_names, meta.dst_offset, meta.field_type,
                       meta.chunk_size, meta.fill_data, meta.compress, this->data());
        table_is_ready = true;

    }
}


void class_hdf5_table_buffer::write_buffer_to_file() {
    if (!this->empty() and table_is_ready) {
        hsize_t NRECORDS = this->size();
        std::string table_path_n = table_path + std::to_string(mpi_rank);
        hdf5_out->create_group_link(group_name);
        H5TBappend_records(hdf5_out->file, table_path_n.c_str(), NRECORDS, meta.dst_size,
                           meta.dst_offset, meta.dst_sizes, this->data());

        this->clear();
    }
    buffer_is_empty = true;
}