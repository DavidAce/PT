#!/usr/bin/env python3
import os
import sys
import fileinput
import decimal

textToReplace = ["T_min=","T_max=", "J1=", "J2="]
fileToSearch  = "../../input/default/default.dat"


with open(fileToSearch, 'r') as infile, open('dummy_new', 'w') as outfile:
    for line in infile:
        if len(line.split()) < 1:
            continue;
        for iteration, (i,j) in enumerate(zip(line.split(),textToReplace)):
            print(i,j)


            if(i == j):
                print(line.split()[1])

        #if (len(line.split()) < 2): continue;
        #print (any(substring in line for substring in textToReplace))
        #print (any(textToReplace == word for word in line.split()))
        #print (line.index(substring) for substring in textToReplace)

        #if (any(substring in line for substring in textToReplace)):

        #for text in textToReplace:
            #print( line.replace(text + line.split()[1], text + "test"))
            #outfile.write(line.replace(text + line.split()[1], text + "test"))
