#!/bin/bash

#SBATCH -J1 WL
#SBATCH -t 0-06:00:00
#SBATCH -N 6
#SBATCH --exclusive
export OMP_NUM_THREADS=1
mpprun ./build/Release/PT
