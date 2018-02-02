#!/bin/bash

#rm -rf *
buildtype="Release"

if [[ "$@" == *"ebug"* ]]
then
	buildtype="Debug"
fi

if [[ "$@" == *"lean"* ]]
then
    echo "Cleaning build"
	rm -rf build
    exit 0
fi


if [[ "$HOSTNAME" == *"triolith"* ]]
then
    echo "We're on triolith!";
    module add cmake/3.6.1
    module load buildenv-intel/2016-3
    export CC=/software/apps/gcc/5.3.0/build01/bin/gcc
    export CXX=/software/apps/gcc/5.3.0/build01/bin/g++
else
    echo "We're on my pc!"
fi
rm -rf build
mkdir build
cd build
mkdir ${buildtype}
cd ${buildtype}
export CC=gcc-7
export CXX=g++-7

echo "Starting Build"
cmake -DCMAKE_C_COMPILER=gcc-7 -DCMAKE_CXX_COMPILER=g++-7 -DCMAKE_BUILD_TYPE=${buildtype}  ../../
make