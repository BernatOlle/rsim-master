#!/bin/bash

mac_protocols=(
brs_non_p
token
fuzzy_token
)

INPUT_DIR=./traces/randomized_traces_burst/iter-4
LOG_DIR=./output/output_burstiness_iter4
RESULTS_DIR=./results/results_burstiness_iter4/data_throughput/
RESULTS_DIR2=./results/results_burstiness_iter4/data_latencies/

rm $RESULTS_DIR -r
rm $RESULTS_DIR2 -r
rm $LOG_DIR

filenames=`ls $INPUT_DIR`
for file in $filenames
do
	echo $file
	for mac_protocol in "${mac_protocols[@]}"
	do
		echo $mac_protocol
		# if we use brs, since it's non-deterministic, we need to repeat experiments multiple times and then we'll do average

		mkdir -p ${LOG_DIR}/${mac_protocol}
		# ./Project ncores=64 inj_rate=$inj_rate mac_protocol=$mac_protocol >> $LOG_DIR
		./Project ncores=64 tx_time=4 mac_protocol=$mac_protocol inj_distribution=trace load_trace_path=${INPUT_DIR}/${file} save_results_path=./results/results_burstiness_iter4 >> ${LOG_DIR}/${mac_protocol}/$file &
	done
	wait
done
