#!/bin/bash

timestart=2

for i in 14000 15000 16000
do
#kill application on the port
	fuser -k $i/tcp 
done

(cd index_connection; ./start.sh &)
sleep $timestart
(cd ranking_server; ./start.sh &)
sleep $timestart
(cd south_protocol; ./start.sh &)
