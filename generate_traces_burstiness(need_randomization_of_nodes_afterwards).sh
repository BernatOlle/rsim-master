#!/bin/bash

mac_protocols=(
# brs_non_p
token
# fuzzy_token
)

H_values=(
0.5
0.6
0.7
0.8
0.9
)

inj_rate_values=(
0.0001
0.0002
0.0003
0.0004
0.0005
0.0006
0.0007
0.0008
0.0009
0.0010
0.0011
0.0012
0.0013
0.0014
0.0015
0.0016
0.0017
0.0018
0.0019
0.0020
0.0021
0.0022
0.0023
0.0024
0.0025
0.0026
0.0027
0.0028
0.0029
0.0030
0.0031
0.0032
0.0033
0.0034
0.0035
0.0036
0.0037
0.0038
0.0039
0.0040
0.0045
0.0050
0.0055
0.0060
0.0065
)


LOG_DIR=./output/output_generating_traces_burst
RESULTS_DIR=./results/results_generating_traces_burst/data_throughput/
RESULTS_DIR2=./results/results_generating_traces_burst/data_latencies/

rm $RESULTS_DIR -r
rm $RESULTS_DIR2 -r
rm $LOG_DIR
touch $LOG_DIR

for H in "${H_values[@]}"
do
	echo Hurst: $H
	for mac_protocol in "${mac_protocols[@]}"
	do
		echo $mac_protocol
		for inj_rate in "${inj_rate_values[@]}"
		do
			echo injection rate: $inj_rate
			for i in {1..5}
			do
				echo iteration: $i
				# ./Project ncores=64 inj_rate=$inj_rate mac_protocol=$mac_protocol >> $LOG_DIR
				./Project ncores=64 inj_rate=$inj_rate tx_time=4 npackets=10000 mac_protocol=token inj_distribution=burst sigma=100 H=$H save_results_path=./results/results_generating_traces_burst save_trace=true save_trace_path=./traces/traces_burst/trace_npackets-10000_sigma-100_H-${H}_injrate-${inj_rate}_iter-${i}.txt &> ${LOG_DIR}/${H}_${inj_rate}_${i}.txt &
			done
			wait
		done
	done
done
