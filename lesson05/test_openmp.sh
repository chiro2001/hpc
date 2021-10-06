#!/bin/bash
./build_mkfile/mat_mul_test 256 &
for i in $(seq 1 10)
do
sleep 0.1 && pstree -p `ps -ef | grep "mat_mul_test" | grep -v grep | awk '{print $2}'`
done
