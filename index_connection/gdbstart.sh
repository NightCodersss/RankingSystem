#!/bin/bash

PROJECT_PATH=/home/asio/rankingsystem

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROJECT_PATH/UbjsonCpp/src/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PROJECT_PATH/config_loader/
gdb -ex run $PROJECT_PATH/index_connection/indexserver
