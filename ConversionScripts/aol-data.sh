#!/bin/bash


for FILE in $*; do
	zcat $FILE | cut  -f2 
done | sort |uniq
