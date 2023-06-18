#!/bin/bash
for i in {01..13}
do
    ./n-puzzle < "mhb_tc/in$i.txt" > "mhb_outputs/out$i.txt"
    # diff "mhb_tc/out$i.txt" "mhb_outputs/out$i.txt"
done
