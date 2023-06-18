#!/bin/sh

read pass

if [ $pass = "secret" ]
then
	echo "password correct"
else
	echo "incorrect"
fi
