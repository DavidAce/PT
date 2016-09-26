#!/bin/bash
echo "Running"
# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

if [[ "$@" == *"valgrind"* ]]
then
    valgrind --tool=memcheck --leak-check=full -v mpirun -n 4 ./build/Debug/PT
elif [[ "$@" == *"gdb"* ]]
then
    mpirun -n 4 xterm -e gdb ./build/Debug/PT
#    mpirun -n 4 gdb build/Debug/PT

elif [[ "$@" == *"ebug"* ]]
then
    ulimit -c unlimited
    mpirun -n 4  -bind-to core:overload-allowed ./build/Debug/PT
elif [[ "$@" == *"elease"* ]]
then
    ulimit -c unlimited
    mpirun -n 4  -bind-to core:overload-allowed ./build/Release/PT
else
    ulimit -c unlimited
    mpirun -n 4  -bind-to core:overload-allowed ./build/Release/PT
fi
function ctrl_c() {
        echo "** Trapped CTRL-C"
}



