#!/bin/bash

INPUT_DIR=./traces/traces_burst
OUTPUT_DIR=./traces/randomized_traces_burst

filenames=`ls $INPUT_DIR`
for file in $filenames
do
	# echo ${INPUT_DIR}/$file
	./single_files/randomize_burst_traces_by_hotspotness_weights.o ${INPUT_DIR}/$file ${OUTPUT_DIR}/$file
done
