#!/bin/bash

# 1. return by echo

take_sum()
{
	echo 'this is'
	variable=100
	total=0
	for i in $*
	do
		total=$(($total + $i))
	done
	echo 'result'
	echo $total
	echo $variable
}


result=$(take_sum 1 2 3 4)

echo $result


