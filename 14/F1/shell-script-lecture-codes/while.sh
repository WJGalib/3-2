#!/bin/bash

read password

while [ $password != "hack" ]
do
	echo "Password wrong"
	read password
done
