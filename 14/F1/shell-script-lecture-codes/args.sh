#!/bin/bash

total=0

for i in $*
do
	total=$(($total + $i))
done


echo $total
