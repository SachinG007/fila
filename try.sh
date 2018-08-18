#!/bin/bash
horizon=6 # example
seeds=1

for ((i = 0; i < horizon; i++)); do
  for ((j = 0; j < seeds; j++)); do
  	echo "new settings"
  	echo $i $j
    ./startexperiment.sh $i $j
    echo "extracted part"
    tail -2 serverlog.txt | head -1 | cut -c 1-3,6-
  done
done	