//
// Created by david on 12/5/16.
//

#ifndef PT_CLASS_MODEL_J1J2_H
#define PT_CLASS_MODEL_J1J2_H

#include "class_model.h"
class J1J2: public Imodel<double> {
private:
    int randI,randJ; //Coordinates of random position;
public:

    J1J2(int l, double j1, double j2): Imodel<double>(l,j1,j2){
        lattice.resize(L,L);
        randomize_lattice();
        set_E_initial();
        set_M_initial();
    };

    virtual void randomize_lattice() final override;
    virtual void flip()              final override {
        lattice(randI,randJ) *= -1;
    }

    inline int sum_neighbours(const int i, const int j) __attribute__((always_inline)){
        return            lattice(math::mod(i+1,L), math::mod(j  ,L))
                          +    lattice(math::mod(i  ,L), math::mod(j+1,L))
                          +    lattice(math::mod(i-1,L), math::mod(j  ,L))
                          +    lattice(math::mod(i  ,L), math::mod(j-1,L));
    }

    virtual void make_new_state()    final override {
        randI = rn::uniform_integer(0, L-1);
        randJ = rn::uniform_integer(0, L-1);
        E_trial = E + J1 * 2 *  lattice(randI, randJ) * sum_neighbours(randI, randJ);
        M_trial = M - 2 *      lattice(randI, randJ);
    };



    virtual inline void  __attribute__ ((always_inline)) accept_state() final override{
        E = E_trial;
        M = M_trial;
        flip();
    }

    virtual void reject_state(){
    }

    void set_E_initial();
    void set_M_initial();
    friend std::ostream &operator<<(std::ostream &, const J1J2 &);


};

#endif //PT_CLASS_MODEL_J1J2_H
