//
// Created by david on 10/18/16.
//

#ifndef PT_THERMODYNAMICS_H
#define PT_THERMODYNAMICS_H

namespace thermo{
    extern void specific_heat (class_worker &);
    extern void susceptibility(class_worker &);
    extern void flyvbjerg(class_worker &);
    extern void block_operation();
}



#endif //PT_THERMODYNAMICS_H
