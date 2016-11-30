#!/usr/bin/env python3
import os
import sys
import itertools

model = sys.argv[1:] if sys.argv[1:] else 'default'
print (model)
outputPath = "../../input/" + model + "/"
fileToSearch  = "../../input/template.dat"
textToReplace = ["T_min=","T_max=", "J1=", "J2=", "L=", "MCS_warmup", "MCS_sample"]
T_min_file  =  open('T_min.dat', 'r').read().splitlines()
T_max_file  =  open('T_max.dat', 'r').read().splitlines()
J1_file     =  open('J1.dat', 'r').read().splitlines()
J2_file     =  open('J2.dat', 'r').read().splitlines()
L_file      =  open('L.dat', 'r').read().splitlines()
MCS_warmup_file  =  open('MCS_warmup.dat', 'r').read().splitlines()
MCS_sample_file  =  open('MCS_sample.dat', 'r').read().splitlines()

for (T_min,T_max, J1, J2,L, MCS_warmup, MCS_sample) in itertools.product(T_min_file, T_max_file, J1_file, J2_file, L_file, MCS_warmup_file, MCS_sample_file):
        fileToWrite = outputPath + model + '_L[' + L + ']_J1[' + J1 + ']_J2['  + J2 +  '].dat'
        os.makedirs(os.path.dirname(fileToWrite), exist_ok=True)

        with open(fileToSearch, 'r') as infile, open(fileToWrite, 'w') as outfile:
            T_min_adj = T_min if (float(J1) == 0) else str(float(T_min) / float(J1))
            T_max_adj = T_max if (float(J1) == 0) else str(float(T_max) / float(J1))
            for line in infile:
                line_array  = line.split();
                newline = line;
                if len(line_array) > 1:
                    newline  = newline.replace(textToReplace[0] + " " + line_array[1], textToReplace[0] + " " + T_min_adj)
                    newline  = newline.replace(textToReplace[1] + " " + line_array[1], textToReplace[1] + " " + T_max_adj)
                    newline  = newline.replace(textToReplace[2] + " " + line_array[1], textToReplace[2] + " " + J1)
                    newline  = newline.replace(textToReplace[3] + " " + line_array[1], textToReplace[3] + " " + J2)
                    newline  = newline.replace(textToReplace[4] + " " + line_array[1], textToReplace[4] + " " + L)
                    newline  = newline.replace(textToReplace[5] + " " + line_array[1], textToReplace[5] + " " + MCS_warmup)
                    newline  = newline.replace(textToReplace[6] + " " + line_array[1], textToReplace[6] + " " + MCS_sample)
                outfile.write(newline)
