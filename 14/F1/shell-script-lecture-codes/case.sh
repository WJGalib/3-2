#!/bin/sh

echo "Have you eaten?"
read ans

case $ans in
	y|Y|y*|Y*) echo "Good!"; echo "Eat More";;
	n|N|n*|N*) echo "Please have these cookies.";;
	*) echo "Can't understand.";;
esac
	
