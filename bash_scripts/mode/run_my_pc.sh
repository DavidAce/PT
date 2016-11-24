#!/bin/bash
# trap ctrl-c and call ctrl_c()
trap ctrl_c INT
function is_int() { return $(test "$@" -eq "$@" > /dev/null 2>&1); }
ARG1=${1:-"Release"}
ARG2=${2:-"default"}
for var in "$@"
do
    if $(is_int "$var");
    then 
        echo "Number of cores: $var"
        NP=$var
    fi
done
NP=${12-NP}
NUMARGS=$#
echo "Arguments ($NUMARGS): $@" 


if [[ NUMARGS -le 1 ]]
then
    if [[ $ARG1 == *"valgrind"* ]]
    then
        echo "Running PT with option valgrind"
        valgrind --tool=memcheck --leak-check=full -v mpirun -n 4 ./build/Debug/PT
    elif [[ $ARG1 == *"gdb"* ]]
    then
        echo "Running PT with option gdb"
        mpirun -n 12 xterm -e gdb ./build/Debug/PT

    elif [[ $ARG1 == *"ebug"* ]]
    then
        ulimit -c unlimited
        echo "Running PT in Debug-mode"
        mpirun -n 12  -bind-to core:overload-allowed ./build/Debug/PT
    elif [[ $ARG1 == *"elease"* ]]
    then
        echo "Running PT in Release-mode"
        ulimit -c unlimited
        mpirun -n 12  -bind-to core:overload-allowed ./build/Release/PT
    elif [[ $ARG1 == *"atch" ]]
    then
        echo "Use syntax: ./run.sh batch [jobname]"
    else 
        echo "WTF? "
    fi
elif [[ NUMARGS -gt 1 ]]
then
    if [[ $ARG1 == *"atch"* ]]
    then
        echo "Running PT in Batch-mode"
        ulimit -c unlimited
        for filename in input/$ARG2/*.dat; do
            echo "Executing file: $filename"
            mpirun -n 12  -bind-to core:overload-allowed ./build/Release/PT "$filename"
        done
    else 
        echo "Use syntax: ./run.sh batch [jobname]"
    fi
fi

function ctrl_c() {
        echo "** Trapped CTRL-C"
}



