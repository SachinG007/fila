#!/bin/bash
seeds=100
# rm ./out.txt
# output=./out.txt

for ((j = 0; j < seeds; j++)); do
	echo $j
	./startexperiment.sh $j	
done


