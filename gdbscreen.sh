#!/bin/bash

./stopservers.sh
killall -15 screen

screen -dmS "Index" 
screen -r "Index" -X stuff $". $RANKING_SYSTEM_HOME/export_config.sh\n"
screen -r "Index" -X stuff $"$RANKING_SYSTEM_HOME/index_connection/gdbstart.sh\n"
sleep 1

screen -dmS "Ranking" 
screen -r "Ranking" -X stuff $". $RANKING_SYSTEM_HOME/export_config.sh\n"
screen -r "Ranking" -X stuff $"$RANKING_SYSTEM_HOME/ranking_server/gdbstart.sh\n"
sleep 1

screen -dmS "South" 
screen -r "South" -X stuff $". $RANKING_SYSTEM_HOME/export_config.sh\n"
screen -r "South" -X stuff $"$RANKING_SYSTEM_HOME/south_protocol/gdbstart.sh\n"
sleep 1

#screen -dmS "Ranking" "./ranking_server/gdbstart.sh" ""; sleep 1;
#screen -dmS "South" "./south_protocol/gdbstart.sh" ""; sleep 1;
