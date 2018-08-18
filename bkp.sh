#!/bin/bash
horizon=6 # example
seeds=4

for ((i = 0; i < horizon; i++)); do
	total_reg=0
  for ((j = 0; j < seeds; j++)); do
  	# echo "new settings"
  	# echo $i $j
    ./startexperiment.sh $i $j
    # echo "extracted part"
    regret="$(tail -2 serverlog.txt | head -1 | cut -c 10-)"
    echo $regret
    echo "total"
    # echo $total_reg + $regret | bc
    total_reg="$( bc <<<"$regret + $total_reg" )"
    # total_reg="$($total_reg + $regret | bc)"
    # total_reg=$((total_reg+regret) | bc
    # echo "cureent total"
    echo $total_reg
  done
done	