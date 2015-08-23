#!/bin/bash

python generate.py
for i in $(ls index_*.dat)
do
	echo $i
	s=$(sort -n -t " " -k 3 $i | tac )
	echo $s > $i
done
