#!/bin/bash

if [ -z "$MAIN_EXPORTS_ARE_DONE" ] 
then
	echo "Make exports";
	exit 1;
fi
./stopservers.sh
make || { echo "Compilation error"; exit 1; }
./runservers.sh
