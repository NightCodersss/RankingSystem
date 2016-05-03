#!/bin/bash
find $PRESSA_HOME -name "*.TXT" | ./index.py commit hashes 
