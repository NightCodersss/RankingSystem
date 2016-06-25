#!/bin/bash

find $PRESSA_HOME -name *.TXT | head -n 100000 | shuf | head -n 100 | python idf.py idf
