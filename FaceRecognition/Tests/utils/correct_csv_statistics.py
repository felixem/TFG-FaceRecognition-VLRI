#!/usr/bin/env python

import sys
import os.path

# This is a tiny script to help you creating a CSV file from a face
# database with a similar hierarchie:
#
#  philipp@mango:~/facerec/data/at$ tree
#  .
#  |-- README
#  |-- s1
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#  |-- s2
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#  ...
#  |-- s40
#  |   |-- 1.pgm
#  |   |-- ...
#  |   |-- 10.pgm
#

def is_number(s):
    try:
        n=str(float(s))
        if n == "nan" or n=="inf" or n=="-inf" : return False
    except ValueError:
        try:
            complex(s) # for complex
        except ValueError:
            return False
    return True

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print ("usage: " +sys.argv[0]+ "<base_path>")
        sys.exit(1)

    BASE_PATH=sys.argv[1]
    SEPARATOR=";"

    for dirname, dirnames, filenames in os.walk(BASE_PATH):
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                if("-output" in filename):
                    os.remove(os.path.join(subject_path,filename))
                
    for dirname, dirnames, filenames in os.walk(BASE_PATH):
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                infile = open(os.path.join(subject_path,filename))
                splitname = filename.split('.')
                outfile = open(os.path.join(subject_path,splitname[0]+"-output."+splitname[1]),"w")
                for line in infile:
                    split = line.split(';')
                    if(len(split) > 1 and is_number(split[1])):
                        vPos = float(split[2])
                        posConf = float(split[3])
                        falsNen = float(split[6])
                        fcpr = posConf / (vPos+posConf+falsNen)
                        split[8] = str(fcpr)
                        nuevaLinea = ";".join(split)
                        outfile.write(nuevaLinea)
                    else:
                        outfile.write(line)

    for dirname, dirnames, filenames in os.walk(BASE_PATH):
        for subdirname in dirnames:
            subject_path = os.path.join(dirname, subdirname)
            for filename in os.listdir(subject_path):
                if("-output" not in filename):
                    os.remove(os.path.join(subject_path,filename))
