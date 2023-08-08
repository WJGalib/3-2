#!/bin/bash
gcc -pthread ipc.c student.c group.c staff.c -o ipc -lgsl -lgslcblas -lm
./ipc < input.txt 
