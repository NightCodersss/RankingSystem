#!/bin/bash

cd $RANKING_SYSTEM_HOME/ranking_server
gdb -ex "run config_file" rankingserver
