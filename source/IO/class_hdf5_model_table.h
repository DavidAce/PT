//
// Created by david on 2018-02-07.
//

#ifndef PT_CLASS_HDF5_MODEL_TABLE_H
#define PT_CLASS_HDF5_MODEL_TABLE_H
#include <iostream>
#include <vector>
#include <memory>
#include <hdf5_hl.h>
//#include <IO/class_hdf5_file.h>
class class_hdf5_file;


struct model_entry{
    double   J1;
    double   J2;
    double   J3;
    int      L;


    model_entry(
            double   J1,
            double   J2,
            double   J3,
            int       L )
            :
            J1(J1),
            J2(J2),
            J3(J3),
             L( L)
             {}
};



class model_entry_meta{
public:
    constexpr static hsize_t NFIELDS = 4;
    size_t dst_size = sizeof( model_entry );
    size_t dst_offset[NFIELDS] = { HOFFSET(model_entry, J1),
                                   HOFFSET(model_entry, J2),
                                   HOFFSET(model_entry, J3),
                                   HOFFSET(model_entry,  L)
                                   };

    size_t dst_sizes[NFIELDS] = {  sizeof(model_entry::J1),
                                   sizeof(model_entry::J2),
                                   sizeof(model_entry::J3),
                                   sizeof(model_entry::L)
    };

    const char *field_names[NFIELDS] =
            { "J1",
              "J2",
              "J3",
              "L"
            };
    /* Define field information */
    hid_t      field_type[NFIELDS] = {H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_INT };
    hid_t      string_type = H5Tcopy( H5T_C_S1 );
    hsize_t    chunk_size  = NFIELDS;
    int        *fill_data  = NULL;
    int        compress    = 0;
    int        i;
    model_entry_meta(){
        H5Tset_size( string_type, 16 );
    }
};

class class_hdf5_model_table : public std::vector<model_entry> {
public:
    std::shared_ptr<class_hdf5_file> hdf5_out;
    model_entry_meta meta;
    std::string group_name      = "default_group";
    std::string table_name      = "default_table";
    std::string table_path;
    int mpi_rank, mpi_size;
    hsize_t recorded_elements       = 0;
    bool buffer_is_empty = false;
    bool table_is_ready  = false;
    bool mpi_on          = false;
    explicit class_hdf5_model_table()=default;
    class_hdf5_model_table(std::shared_ptr<class_hdf5_file> hdf5_out_,
    std::string group_name_,
            std::string table_name,
    bool mpi_on_   );
    class_hdf5_model_table(std::nullptr_t nullp,
    std::string group_name_,
            std::string table_name,
    bool mpi_on_);

    class_hdf5_model_table(std::string group_name_,
    std::string table_name_,
    bool mpi_on_  );
    void initialize_table();
    void initialize_table_mpi();
    void write_buffer_to_file();
    void write_buffer_to_file_mpi();

    ~class_hdf5_model_table(){
        if (hdf5_out){
            if(mpi_on){
                write_buffer_to_file_mpi();
            }
            else {
                write_buffer_to_file();
            }
        }else if (!buffer_is_empty){
            std::cerr << "Warning: Output data has not been saved to file, yet it is being discarded!\n" << std::endl;
        }
    }
};


#endif //PT_CLASS_HDF5_MODEL_TABLE_H
