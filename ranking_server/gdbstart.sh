#!/bin/bash

PROJECT_PATH=/home/asio/rankingsystem

export LC_ALL=C
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROJECT_PATH/UbjsonCpp/src/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROJECT_PATH/config_loader/
cd $PROJECT_PATH/ranking_server
gdb -ex "run config_file" rankingserver
