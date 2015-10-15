#!/bin/bash

python makeindex_random.py
../util/sortindex.sh

sort -r -n -t " " -k 2 checkfile -o checkfile
