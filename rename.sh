#!/bin/bash

cat $1 | while read line
do
	IFS=" " read -r -a replacement <<< "$line";
	echo $line
	from=(${replacement[0]})
	to=(${replacement[1]})

	echo "Replace $from to $to"

	for filename in `find -name \*.cpp` `find -name \*.hpp`
	do
		sed -i -e "s/$from/$to/g" $filename
	done
	
	for name in `ls *$from*` 
	do
	mv "$name" "${name/$from/$to}"
	done
done
