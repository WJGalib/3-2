#!/bin/sh


# 3. return

take_sum()
{
	total=0
	for i in $*
	do
		total=$(($total + $i))
	done
	return $total
}

take_sum2()
{
	total=0
	for i in $*
	do
		total=$(($total + $i))
	done
}

take_sum 1 2 3 4

`take_sum2 1 2 3 6`

echo $?
