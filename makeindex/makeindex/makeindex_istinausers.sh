#!/bin/bash

DATA_FILE= /home/asio/rankingsystem/makeindex/data/istinausers.txt

for i in $(seq 3)
do
	sort -n -t "|" -k 3 $DATA_FILE | tail -$(expr 100 \* "$i") | head -100 | tac | sed 's/|//g' > index."$i".index
done
