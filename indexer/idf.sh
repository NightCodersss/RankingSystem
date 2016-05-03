#!/bin/bash

find $PRESSA_HOME -name *.TXT | head -n 100000 | shuf | head -n 20000 | python idf.py idf
