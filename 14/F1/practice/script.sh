#!/bin/bash
a=1
lslist=$(ls)
echo $lslist
for i in lslist 
do
    echo "The $a No arg is ${lslist[$i]}"
    a=`expr $a + 1`
done
