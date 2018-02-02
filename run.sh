#!/bin/bash
option=$@
echo "Starting job"
if [[ "$OSTYPE" == "linux-gnu" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    echo "OS: Linux"
    if [[ "$HOSTNAME" == *"triolith"* ]]; then
        echo "We're on triolith!";
        if [[ "${option}" == *"valgrind"* ]]; then
            sbatch bash_scripts/mode/run_triolith_debug.sh
        else
            sbatch bash_scripts/mode/run_triolith.sh
        fi
    else
        bash_scripts/mode/run_linux.sh ${option}
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "OS: Mac OSX"
    bash_scripts/mode/run_macosx.sh ${option}
else
        echo "Could not identify OS"
fi
