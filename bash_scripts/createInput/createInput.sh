#!/bin/bash
if [ $# -eq 0 ]
  then
    echo "Please provide model name as argument, e.g.: './createInput.sh my_model'"
    exit 1
fi

model=$@
for T_min in `cat T_min.dat`; do
    for T_max in `cat T_max.dat`; do
        for L_row in `cat L.dat`; do
            for J1_row in `cat J1.dat`; do
                for J2_row in `cat J2.dat`; do
                    in_file='../../input/'${model}'/L['$L_row']_J1['$J1_row']_J2['$J2_row'].dat'
                    mkdir -p "$(dirname $in_file)"
                    echo $J1_row
                    ratio=3 # $(($T_min/$J1_row))
                    Tmin=$(($J1_row==0?7:$ratio))
                    Tmax=$((J1_row != 0 ? T_max : 10))
                    cat ../../input/default/default.dat      | \
                        sed  "s%T_min= [-.0-9]*%L= $Tmin%"   | \
                        sed  "s%T_max= [-.0-9]*%L= $Tmax%"   | \
                        sed  "s%L= [-.0-9]*%L= $L_row%"      | \
                        sed  "s%J1= [-.0-9]*%J1= $J1_row%"   | \
                        sed  "s%J2= [-.0-9]*%J2= $J2_row%"   > $in_file
                    sleep 0.01
                done
            done
        done
    done
done



