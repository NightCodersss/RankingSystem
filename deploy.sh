#!/bin/bash

./stopservers.sh
make || { echo "Compilation error"; exit 1; }
./runservers.sh
