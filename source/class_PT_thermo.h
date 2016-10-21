//
// Created by david on 10/18/16.
//

#ifndef PT_CLASS_PT_THERMO_H
#define PT_CLASS_PT_THERMO_H
#include "nmspc_PT_constants.h"
#include "class_PT_worker.h"

class thermodynamics{
private:
    int world_ID;
    double T;
    ArrayXXd read_file(string);
    ArrayXd E,M;



public:
    thermodynamics(int id, double t): world_ID(id), T(t){};
    double u, sigma_u;
    double c, sigma_c;
    double x, sigma_x;

    void   load_data();
    void   bootstrap_error  ();
    double flyvbjerg_error  (const ArrayXd & A);
    void   block_transform  (ArrayXd & B);

    void   internal_energy      ();
    void   specific_heat        ();
    void   susceptibility       ();
    double internal_energy      (const ArrayXd &E);
    double specific_heat        (const ArrayXd &E);
    double susceptibility       (const ArrayXd &M);

};



#endif //PT_CLASS_PT_THERMO_H
