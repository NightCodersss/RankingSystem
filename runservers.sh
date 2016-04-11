#!/bin/bash

timestart=2

export LC_ALL=C

(cd index_connection; ./start.sh &)
(cd forward_index_server; ./start.sh &)
(cd force_ranking_server; ./start.sh &)
sleep $timestart
(cd ranking_server; ./start.sh &)
sleep $timestart
(cd south_protocol; ./start.sh &)
