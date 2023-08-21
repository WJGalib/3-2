#!/bin/bash
printf 'Input_Filename,|V|,|E|,randomised,simple_greedy,semi-greedy,Iterations,Best Value,Iterations,Best Value\n'
for i in {1..54}; do
    ./max_cut "set1/g$i.rud" 50
done