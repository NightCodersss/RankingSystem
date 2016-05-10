#!/bin/bash

for i in $SOUTH_PORT $INDEX_PORT $RANKING_PORT $FORWARD_PORT $FORCE_RANKING_PORT $STEM_PORT
do
#kill application on the port
	fuser -k $i/tcp 
done
echo "Stopped"

