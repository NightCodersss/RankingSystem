#!/bin/bash

python generate.py
for i in $(ls index_*.dat)
do
	sort -r -n -t " " -k 3 $i -o $i
done

sort -r -n -t " " -k 2 checkfile -o checkfile
