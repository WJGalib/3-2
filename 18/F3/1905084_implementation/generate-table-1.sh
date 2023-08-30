#!/bin/bash
for i in {1..24}; do
    ./max_cut "set1/g$i.rud" 50
done