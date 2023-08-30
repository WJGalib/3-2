#!/bin/bash
g++ max_cut.cpp -O9 -o max_cut
printf ',Problem,,,Constructive Algorithm,,Local Search,,,GRASP,\n'
printf 'Input Filename,|V|,|E|,randomised,simple_greedy,semi-greedy,Iterations,Best Value,Iterations,Best Value,Constructive Algorithm\n'
for i in {1..54}; do
    ./max_cut "set1/g$i.rud" 50
done