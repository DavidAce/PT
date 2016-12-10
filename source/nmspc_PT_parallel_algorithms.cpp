//
// Created by david on 2016-08-11.
//

#include "nmspc_PT_parallel_algorithms.h"
#define debug_swap      0

using namespace std;

namespace parallel {
    void swap(class_worker &worker) {
        //Use MPI Tag in the 100-200 range
        timer::swap = 0;
        worker.t_swap.tic();
        int swap;
        double E_up;
        double P_swap;      //Swap probability
        bool myTurn = math::mod(worker.T_ID, 2) == math::mod(counter::swap_trials, 2);
        if (debug_swap) {
            for (int w = 0; w < worker.world_size; w++) {
                if (w == worker.world_ID) {
                    cout << "ID: " << w << " Starting Swap. Myturn = " << myTurn << endl;
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        //Send current E neighbors dn and receive from above.
        MPI_Sendrecv(&worker.model->E, 1, MPI_DOUBLE, worker.world_ID_dn, 100, &E_up, 1, MPI_DOUBLE, worker.world_ID_up, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Now both swappees need to know if it is ok to go ahead with a swap.
        if (debug_swap) {
            for (int w = 0; w < worker.world_size; w++) {
                if (w == worker.world_ID) {
                    cout << "ID: " << w << " SendRecv Successful"
                         << " E_up = " << E_up
                         << endl;
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }

        if (myTurn) {
            double T_up = worker.T_ladder(math::mod(worker.T_ID + 1, worker.world_size));
            P_swap = exp((worker.model->E - E_up)*(1 / worker.T - 1 / T_up)); //Swap probability
            swap = rn::uniform_double_1() < fmin(1, P_swap) ? 1 : 0;
            if (worker.T_ID == worker.world_size -1 ){ swap = 0;}
            MPI_Send(&swap, 1, MPI_INT, worker.world_ID_up, 110, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&swap, 1, MPI_INT, worker.world_ID_dn, 110, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if (debug_swap) {
            for (int w = 0; w < worker.world_size; w++) {
                if (w == worker.world_ID) {
                    cout << "ID: " << w << " Swap = " << swap  << endl;
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        if(debug_swap){debug_print(worker,"Starting swap\n");}

        //Communicate the new world_ID before swapping

        int world_ID_dn_new = worker.world_ID_dn, world_ID_up_new = worker.world_ID_up;
        if(myTurn){
            //Receive news from world_ID_dn about who will be your new world_ID_dn
            MPI_Recv(&world_ID_dn_new, 1, MPI_INT, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //(*)I know that if I'm swapping, the variable above is not actually correct yet.
            //world_ID_dn_new will actually by my current up_new

        }else {
            //Send news up to world_ID_up that world_ID_dn will be at this position
            if (swap) {
                MPI_Send(&worker.world_ID_dn, 1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
            } else {
                MPI_Send(&worker.world_ID,    1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
            }
        }

        if(myTurn) {
            if (swap) {
                //Send up
                MPI_Send(&world_ID_dn_new, 1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
                //Now I can correct (*)
                world_ID_dn_new = worker.world_ID_up;
            } else {
                MPI_Send(&worker.world_ID, 1, MPI_INT, worker.world_ID_up, worker.world_ID, MPI_COMM_WORLD);
            }
        }else{
                MPI_Recv(&world_ID_dn_new, 1, MPI_INT, worker.world_ID_dn, worker.world_ID_dn, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        }

        if(myTurn) {
            //Send news dn to world_ID_dn who will be his new neighbor up
            if(swap){
                MPI_Send(&worker.world_ID_up, 1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
            }else{
                MPI_Send(&worker.world_ID   , 1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
            }
        }else{
            MPI_Recv(&world_ID_up_new, 1, MPI_INT, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //(**)I know that if I'm swapping the variable above is not actually correct yet.
            //world_ID_up_new will actually be worker.world_ID_dn
        }

        if(myTurn){
            //Receive news from world_ID_up about who will be your new world_ID_up
            MPI_Recv(&world_ID_up_new, 1, MPI_INT, worker.world_ID_up, worker.world_ID_up, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }else {
            //Send news up to world_ID_dn who will be at this position
            if (swap) {
                MPI_Send(&world_ID_up_new, 1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
                //Now I can correct (**)
                world_ID_up_new = worker.world_ID_dn;
            } else {
                MPI_Send(&worker.world_ID,    1, MPI_INT, worker.world_ID_dn, worker.world_ID, MPI_COMM_WORLD);
            }

        }

        //Katzgraber histogram update
        if(!worker.sampling) {
            if (worker.T_ID == 0 ){
                worker.direction = 1;
            }
            else
            if (worker.T_ID == worker.world_size-1){
                worker.direction = -1;
            }
            worker.n_history.push_back(worker.direction);
            worker.t_history.push_back(worker.T_ID);
        }

        //Now do the swapping if you got lucky
        if (swap == 1) {
            if (myTurn == 1) {
                worker.T_ID = math::mod(worker.T_ID + 1, worker.world_size);
                worker.T = worker.T_ladder(worker.T_ID);
                MPI_Sendrecv_replace(&counter::accepts,1,MPI_INT, worker.world_ID_up, 1, worker.world_ID_up, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

           }
            else {
                worker.T_ID = math::mod(worker.T_ID - 1, worker.world_size);
                worker.T = worker.T_ladder(worker.T_ID);
                MPI_Sendrecv_replace(&counter::accepts,1,MPI_INT, worker.world_ID_dn, 1, worker.world_ID_dn, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


            }
        }

        worker.world_ID_dn = world_ID_dn_new;
        worker.world_ID_up = world_ID_up_new;

        counter::swap_accepts += swap;
        counter::swap_trials++;

        if (debug_swap) {
            for (int w = 0; w < worker.world_size; w++) {
                if (w == worker.world_ID) {
                    cout << "ID: " << w << " Swap OK " << endl;
                }
                MPI_Barrier(MPI_COMM_WORLD);
            }
        }
        worker.t_swap.toc();

    }





    void sort(class_worker &worker, output &out, bool force){
        timer::sort = 0;
        if (worker.sampling || force){
            //Reorder E
            for (int i = 0; i < worker.E_history.size(); i++){
                MPI_Sendrecv_replace(&worker.E_history[i], 1, MPI_DOUBLE, worker.T_history[i],i, MPI_ANY_SOURCE,i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            //Reorder M
            for (int i = 0; i < worker.M_history.size(); i++){
                MPI_Sendrecv_replace(&worker.M_history[i], 1, MPI_DOUBLE, worker.T_history[i],i, MPI_ANY_SOURCE,i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            out.store_samples(worker.E_history, "E" +  std::to_string(worker.world_ID) + ".dat",counter::store);
            out.store_samples(worker.M_history, "M" +  std::to_string(worker.world_ID) + ".dat",counter::store);
            worker.E_history.clear();
            worker.M_history.clear();
            worker.T_history.clear();
            counter::store++;
        }
    }

    void calc(class_worker &worker, bool force){
        timer::calc = 0;
        worker.thermo.load_data(worker.T_ID, worker.T);
        worker.thermo.calc_thermo();
    }

    void katz(class_worker &worker) {
        timer::move = 0;
        if (!worker.sampling) {
            //Reorder n_history (Katzgraber histogram for T-placement)
            for (int i = 0; i < worker.n_history.size(); i++){
                MPI_Sendrecv_replace(&worker.n_history[i], 1, MPI_INT, worker.t_history[i],i, MPI_ANY_SOURCE,i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            MPI_Barrier(MPI_COMM_WORLD);
            ArrayXd f_all(worker.world_size);
            int nup = 0, ndn = 0;
            for (int i = 0; i < worker.n_history.size(); i++){
                if (worker.n_history[i] == 1){
                    nup++;
                }
                if (worker.n_history[i] == -1) {
                    ndn++;
                }
            }

            double fn = (double)nup/fmax(1,(nup+ndn));
            MPI_Allgather(&fn,1,MPI_DOUBLE, f_all.data(),1,MPI_DOUBLE,MPI_COMM_WORLD);

            if (worker.world_ID == 0) {
                cout << "f before: " << f_all.transpose() << endl;
                cout.flush();
                std::this_thread::sleep_for(std::chrono::microseconds(100));

            }
            MPI_Barrier(MPI_COMM_WORLD);


            //Make f decreasing
            for (int i = 0; i < f_all.size() - 1 ; i++){
                f_all(i+1) = fmin(f_all(i+1), f_all(i));
            }


            //Use Laplacian(?) smoothing: Each interior point is the mean of its neighbors.
            ArrayXd f_smooth = f_all;
            int smooth_iterations = 10;
            for (int s = 0; s < smooth_iterations ; s++) {
                for (int i = 1; i < f_all.size()-1; i++) {
//                    f_smooth(i) = 0.5*(f_all(i-1) + f_all(i+1));
                    f_smooth(i) = (0.1*f_all(i-1) + 0.8*f_all(i) + 0.1*f_all(i+1));

                }
                f_all = f_smooth;
            }


            //Interpolation
            int num_interp = 100;
            vector<double> t_interp, f_interp;
            for (int i = 1; i < worker.world_size; i++) {
                for (int j = 0; j < num_interp; j++) {
                    t_interp.push_back((double) j / num_interp * (worker.T_ladder(i) - worker.T_ladder(i - 1)) + worker.T_ladder(i - 1));
                    f_interp.push_back((double) j / num_interp * (f_all(i) - f_all(i - 1)) + f_all(i - 1));
                }
            }

            //Turn back to an Eigen Array
            ArrayXd f = Map<ArrayXd>(f_interp.data(), f_interp.size());
            ArrayXd t = Map<ArrayXd>(t_interp.data(), t_interp.size());
            //Find intersections
            ArrayXd f_linear = ArrayXd::LinSpaced(worker.world_size, f.maxCoeff(), f.minCoeff());
            for (int i = 1; i < worker.world_size - 1; i++) {
                int idx;
                (f - f_linear(i)).cwiseAbs().minCoeff(&idx);
                worker.T_ladder(i) = t(idx);
            }

            worker.T = worker.T_ladder(worker.T_ID);
            worker.thermo.T = worker.T;

//            for (int w = 0; w < worker.world_size; w++){
//                if(w == worker.T_ID){
//                    cout << counter::accepts/(double)counter::trials << " ";
//                    cout.flush();
//                    std::this_thread::sleep_for(std::chrono::microseconds(100));
//
//                }
//                MPI_Barrier(MPI_COMM_WORLD);
//            }
//            MPI_Barrier(MPI_COMM_WORLD);
//            if (worker.world_ID == 0){
//                cout << endl;
//            }

            if (worker.world_ID == 0) {
                cout << "f after : "<< f_all.transpose() << endl;
                cout << worker.T_ladder.transpose() << endl << endl;
                cout << counter::accepts/(double)counter::trials << " ";
                cout.flush();
                std::this_thread::sleep_for(std::chrono::microseconds(100));

            }
            MPI_Barrier(MPI_COMM_WORLD);
            worker.n_history.clear();
            worker.t_history.clear();
            counter::accepts = 0;
            counter::trials = 0;
            counter::swap_accepts = 0;
            counter::swap_trials = 0;


        }
    }

    void save(class_worker &worker, output & out){
        worker.thermo.load_data(worker.T_ID, worker.T);
        worker.thermo.calc_thermo();
        int n = worker.world_size;
        ArrayXd u(n),m(n),c(n),x(n);
        ArrayXd sigma_u(n),sigma_m(n),sigma_c(n),sigma_x(n);
        int color, key;
        MPI_Comm MPI_COMM_T;
        color = 0;
        key   = worker.T_ID;
        MPI_Comm_split(MPI_COMM_WORLD, color, key, &MPI_COMM_T);

        MPI_Gather(&worker.thermo.u, 1, MPI_DOUBLE, u.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);
        MPI_Gather(&worker.thermo.m, 1, MPI_DOUBLE, m.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);
        MPI_Gather(&worker.thermo.c, 1, MPI_DOUBLE, c.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);
        MPI_Gather(&worker.thermo.x, 1, MPI_DOUBLE, x.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);

        MPI_Gather(&worker.thermo.sigma_u, 1, MPI_DOUBLE, sigma_u.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);
        MPI_Gather(&worker.thermo.sigma_m, 1, MPI_DOUBLE, sigma_m.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);
        MPI_Gather(&worker.thermo.sigma_c, 1, MPI_DOUBLE, sigma_c.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);
        MPI_Gather(&worker.thermo.sigma_x, 1, MPI_DOUBLE, sigma_x.data(), 1, MPI_DOUBLE, 0, MPI_COMM_T);

        if(worker.T_ID == 0){
            out.store_thermo(u, "u.dat");
            out.store_thermo(m, "m.dat");
            out.store_thermo(c, "c.dat");
            out.store_thermo(x, "x.dat");

            out.store_thermo(sigma_u, "u_std.dat");
            out.store_thermo(sigma_m, "m_std.dat");
            out.store_thermo(sigma_c, "c_std.dat");
            out.store_thermo(sigma_x, "x_std.dat");

            out.store_thermo(worker.T_ladder, "T.dat");
            out.store_thermo(worker.model->J1, "J1.dat");
            out.store_thermo(worker.model->J1, "J2.dat");
            out.store_thermo(worker.model->L, "L.dat");
        }



    }

}


