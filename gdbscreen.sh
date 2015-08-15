#!/bin/bash

./stopservers.sh
killall -15 screen

screen -dmS "Index" "./index_connection/gdbstart.sh" ""; sleep 1;
screen -dmS "Ranking" "./ranking_server/gdbstart.sh" ""; sleep 1;
screen -dmS "South" "./south_protocol/gdbstart.sh" ""; sleep 1;
