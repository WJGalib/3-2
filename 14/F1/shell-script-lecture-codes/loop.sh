#!/bin/bash

total=0
for((i=0;i<=50;i++))
do
	total=$(($total + $i))
done

echo $total
