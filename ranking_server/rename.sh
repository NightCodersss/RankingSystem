#!/bin/bash

PREFIX="south"
NEW_PREFIX="ranking"

for name in `ls $PREFIX*` 
do
	mv "$name" "${name/$PREFIX/$NEW_PREFIX}"
done
