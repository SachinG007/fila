#!/bin/bash
horizon=1000 # example
seeds=100

for ((i = 0; i < horizon; i++)); do
  for ((j = 0; j < seeds; j++)); do
    echo "$i $j"
  done
done