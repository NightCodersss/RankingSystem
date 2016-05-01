#!/bin/bash

clang++ -g -std=c++14 test.cpp block.cpp serialize.cpp filestream.cpp -o test -I../ -lboost_system

rm forward_storage.bin*

echo 10 | ./test
echo 20 | ./test
echo 30 | ./test
echo 40 | ./test
echo 50 | ./test
echo 15 | ./test
echo 25 | ./test
echo 35 | ./test
echo 45 | ./test
#./test 23
