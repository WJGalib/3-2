#!/bin/sh


filepath="/home/tareq/play"

if [ -d $filepath ]
then
	echo "This is a directory"
else
	echo "This is a file"
fi
