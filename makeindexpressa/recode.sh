#!/bin/bash

DOCS_PATH="/home/asio/pressa/mnt/backup2/serg/pressa/I"
files=$(find $DOCS_PATH -iname '*.txt')
echo $files
for file in $files
do
	enc=$(chardet $file | awk -F ":" '{print $2}' | awk -F "(" '{print $1}')
	iconv -f $enc -t utf-8 $file > $file.bak
	mv $file.bak $file
done

