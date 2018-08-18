#!/bin/bash
seeds=100
# rm ./out.txt
# output=./out.txt

  for ((j = 0; j < seeds; j++)); do
  	echo  $j
    ./startexperiment.sh $j	
    regret="$(tail -2 serverlog.txt | head -1 | cut -c 10-)"
    # echo $regret
    total_reg="$( bc <<<"$regret + $total_reg" )"
    # echo $total_reg
  done
  	echo $total_reg
  	echo "$total_reg" >> "$output"

