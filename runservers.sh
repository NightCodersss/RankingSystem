#!/bin/bash

timestart=2

(cd index_connection; ./start.sh &)
sleep $timestart
(cd ranking_server; ./start.sh &)
sleep $timestart
(cd south_protocol; ./start.sh &)
