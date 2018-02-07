//
// Created by david on 2018-01-26.
//

#ifndef CLASS_HDF5_TABLE_BUFFER_H
#define CLASS_HDF5_TABLE_BUFFER_H
#include <iostream>
#include <vector>
#include <memory>
#include <hdf5_hl.h>

class class_hdf5_file;

struct thermo_entry{
    double   u;
    double   u_std;
    double   m;
    double   m_std;
    double   c;
    double   c_std;
    double   x;
    double   x_std;


    thermo_entry(
    double   u,
    double   u_std,
    double   m,
    double   m_std,
    double   c,
    double   c_std,
    double   x,
    double   x_std)
            :
            u(u),
            u_std(u_std),
            m(m),
            m_std(m_std),
            c(c),
            c_std(c_std),
            x(x),
            x_std(x_std) {}
};



class class_thermo_entry_meta{
public:
    constexpr static hsize_t NFIELDS = 8;
    size_t dst_size = sizeof( thermo_entry );
    size_t dst_offset[NFIELDS] = { HOFFSET(thermo_entry, u),
                                   HOFFSET(thermo_entry, u_std),
                                   HOFFSET(thermo_entry, m),
                                   HOFFSET(thermo_entry, m_std),
                                   HOFFSET(thermo_entry, c),
                                   HOFFSET(thermo_entry, c_std),
                                   HOFFSET(thermo_entry, x),
                                   HOFFSET(thermo_entry, x_std)};

    size_t dst_sizes[NFIELDS] = {  sizeof(thermo_entry::u),
                                   sizeof(thermo_entry::u_std),
                                   sizeof(thermo_entry::m),
                                   sizeof(thermo_entry::m_std),
                                   sizeof(thermo_entry::c),
                                   sizeof(thermo_entry::c_std),
                                   sizeof(thermo_entry::x),
                                   sizeof(thermo_entry::x_std)
    };

    const char *field_names[NFIELDS] =
            { "u",
              "u_std",
              "m",
              "m_std",
              "c",
              "c_std",
              "x",
              "x_std"
            };
    /* Define field information */
    hid_t      field_type[NFIELDS] = {H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE,
                                      H5T_NATIVE_DOUBLE
                                        };
    hid_t      string_type = H5Tcopy( H5T_C_S1 );
    hsize_t    chunk_size  = NFIELDS;
    int        *fill_data  = NULL;
    int        compress    = 0;
    int        i;
    class_thermo_entry_meta(){
        H5Tset_size( string_type, 16 );
    }
};



class class_hdf5_thermo_table : public std::vector<thermo_entry> {
public:
    std::shared_ptr<class_hdf5_file> hdf5_out;
    class_thermo_entry_meta meta;
    std::string group_name      = "default_group";
    std::string table_name      = "default_table";
    std::string table_path;
    int mpi_rank, mpi_size;
    hsize_t recorded_elements       = 0;
    bool buffer_is_empty = false;
    bool table_is_ready  = false;
    bool mpi_on          = false;
    explicit class_hdf5_thermo_table()=default;
    class_hdf5_thermo_table(std::shared_ptr<class_hdf5_file> hdf5_out_,
                            std::string group_name_,
                            std::string table_name,
                            bool mpi_on_   );
    class_hdf5_thermo_table(std::nullptr_t nullp,
                            std::string group_name_,
                            std::string table_name,
                            bool mpi_on_);

    class_hdf5_thermo_table(std::string group_name_,
                            std::string table_name_,
                            bool mpi_on_  );
    void initialize_table();
    void initialize_table_mpi();
    void write_buffer_to_file();
    void write_buffer_to_file_mpi();

    ~class_hdf5_thermo_table(){
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








#endif //CLASS_HDF5_TABLE_BUFFER_H
