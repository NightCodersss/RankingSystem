#!/bin/bash

for i in $(seq 3)
do
	sort -n -t "|" -k 3 data.txt | tail -$(expr 100 \* "$i") | head -100 | tac | sed 's/|//g' > index_"$i".txt
done
