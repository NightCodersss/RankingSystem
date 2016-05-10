#!/bin/bash

timestart=2

export LC_ALL=C

#(cd index_connection; ./start.sh &) # plain-text files
(cd index_server; ./start.sh &) # binary files
(cd forward_index_server; ./start.sh &)
(cd force_ranking_server; ./start.sh &)
sleep $timestart
(cd ranking_server; ./start.sh &)
sleep $timestart
(cd south_protocol; ./start.sh &)
sleep $timestart
(cd stem_server; ./start.sh &)
echo "Servers are done"
