#!/bin/bash

ports="14000 15000 16000"

for i in $ports
do
#kill application on the port
	fuser -k $i/tcp 
done

