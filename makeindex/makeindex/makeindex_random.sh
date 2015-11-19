#!/bin/bash

python makeindex_random.py
python ../util/sortindex.py

sort -r -n -t " " -k 2 checkfile -o checkfile
