#!/bin/bash

git pull
make
. ./export_config.sh
./deploy.sh
