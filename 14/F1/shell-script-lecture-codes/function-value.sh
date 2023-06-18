#!/bin/bash

# 2. return by variable

total=2000

take_sum()
{
	for i in $*
	do
		total=$(($total + $i))
	done
}


take_sum 1 2 3 4

echo $total


