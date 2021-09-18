#!/bin/bash
# cd ../data/

pids=( )
start=$(date +%s.%N)
th_num=$2
port=$1

if [ -z "$th_num" ]; then
  th_num=1024
fi
if [ -z "$port" ]; then
  th_num=8001
fi

# for J in `seq 1000`; do

  echo Starting $th_num threads.
  for I in `seq $th_num`; do
    echo ----request $I: `wget --tries=1 -q -O - 127.0.0.1:$port` &
    pid=$!
    pids[$I]=$pid
  done
  echo Started $th_num threads.
  for I in `seq $th_num`; do
    wait ${pids[$I]}
  done
  end=$(date +%s.%N)
  delta=`echo | awk -v start=$start -v end=$end '{printf("%.3f", end-start)}'`

  echo Used $delta s

# done