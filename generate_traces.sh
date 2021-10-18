#!/bin/bash

sigma_values=(
# 0.05
# 0.1
# 0.2
# 0.3
# 0.5
# 10
100
# 1000
)

inj_rate_values=(
0.0001
0.0002
0.0005
0.001
)

H_values=(
0.6
0.7
0.8
0.9
)


LOG_DIR=./output/log_traces.txt
TRACES_DIR=./traces/traces_burst

rm $LOG_DIR
touch $LOG_DIR

rm $RESULTS_DIR -r

for sigma in "${sigma_values[@]}"
do
	for H in "${H_values[@]}"
	do
		echo H value: $H
		for inj_rate in "${inj_rate_values[@]}"
		do
			echo injection rate: $inj_rate
			for i in {1..5}
			do
				echo iteration: $i
				# ./Project ncores=64 inj_rate=$inj_rate mac_protocol=$mac_protocol >> $LOG_DIR
				./Project ncores=64 inj_rate=$inj_rate tx_time=4 npackets=10000 mac_protocol=token H=$H sigma=$sigma inj_distribution=burst save_trace=true save_trace_path=${TRACES_DIR}/trace_npackets-10000_sigma-${sigma}_H-${H}_injrate-${inj_rate}_iter-${i}.txt
			done
		done
	done
done
