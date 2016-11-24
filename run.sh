#!/bin/bash
option=$@
echo "Starting job"
if [[ "$HOSTNAME" == *"triolith"* ]]
then
    echo "We're on triolith!";
    if [[ "${option}" == *"valgrind"* ]]
    then
        sbatch bash_scripts/mode/run_triolith_debug.sh
    else
        sbatch bash_scripts/mode/run_triolith.sh
    fi
else
    echo "We're on my pc!"
    ./bash_scripts/mode/run_my_pc.sh ${option}
fi
