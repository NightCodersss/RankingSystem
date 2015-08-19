#!/bin/bash

PREFIX="ranking"
NEW_PREFIX="generating"

for name in `ls $PREFIX*` 
do
	mv "$name" "${name/$PREFIX/$NEW_PREFIX}"
done
