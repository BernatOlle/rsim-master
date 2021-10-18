#!/bin/bash

mac_protocols=(
brs_non_p
#tdma_fixed
#tdma_weighted
#token
#fuzzy_token
)

inj_rate_values=(
0.0001
0.0005
0.001
0.002
0.003
0.004
0.005
0.006
0.007
0.008
0.009
0.01
)

ncores_values=(
# 16
# 20
# 32
# 36
# 50
64
# 100
# 128
# 150
# 200
# 250
# 300
# 350
# 400
# 450
# 500
# 512
# 550
# 600
# 650
# 700
# 750
# 800
# 850
# 900
# 950
# 1000
# 1024
)


LOG_DIR=./output/log.txt
RESULTS_DIR=./results/data_throughput/
RESULTS_DIR2=./results/data_latencies/

rm $RESULTS_DIR -r
rm $RESULTS_DIR2 -r
rm $LOG_DIR
touch $LOG_DIR
mkdir -p $RESULTS_DIR
mkdir -p $RESULTS_DIR2


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
			./Project ncores=64 inj_rate=$inj_rate npackets=10000 mac_protocol=$mac_protocol
		done
	done
done
