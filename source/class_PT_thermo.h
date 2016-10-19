//
// Created by david on 10/18/16.
//

#ifndef PT_CLASS_PT_THERMO_H
#define PT_CLASS_PT_THERMO_H
#include "class_PT_worker.h"
class thermodynamics{
    int world_ID;
private:
public:
    thermodynamics(int id){
        //Load timeseries

        //Generate bootstrapped series

        //

    };
    void specific_heat   (class_worker &);
    void susceptibility  (class_worker &);
    void flyvbjerg       (class_worker &);
    void block_operation ();
};



#endif //PT_CLASS_PT_THERMO_H
