#!/usr/bin/env python3
import os
import sys
import fileinput

textToSearch = "hej"
textToReplace = "då"
fileToSearch  = "dummy.test"
#fileToSearch = 'D:\dummy1.txt'
lines = []
with fileinput.FileInput(fileToSearch, inplace=False) as infile:
    for line in infile:
        line.replace(textToSearch,textToReplace)
        #print(line.replace(textToSearch, textToReplace), end='')
    lines.append(line)
with open('dummy_new', 'w') as outfile:
    for line in lines:
        outfile.write(line)
