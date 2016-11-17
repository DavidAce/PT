#!/bin/bash


for s_row in `cat size_values.dat`; do
    for c_row in `cat coupling_values.dat`; do
        in_file='../input/input_L'$s_row'_J'$c_row'.dat'

        cat ../input/default.dat                 | \
            sed  "s%L= [-.0-9]*%L= $s_row%"      | \
            sed  "s%J= [-.0-9]*%J= $c_row%"       > $in_file

        sleep 0.01
    done
done




