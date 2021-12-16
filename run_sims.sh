#!/bin/bash

mac_protocols=(
brs_non_p
tdma_fixed
#tdma_weighted
token
fuzzy_token
)

sigma_values=(
#0.5
10
#100
)

h_values=(
0.5
#0.7
#0.85
#0.9
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
0.02
0.03
0.04
0.05
0.07
0.1
0.2
0.5
0.7
0.9
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

for sigma in "${sigma_values[@]}"
do
	for h in "${h_values[@]}"
	do
		for mac_protocol in "${mac_protocols[@]}"
		do
			echo $mac_protocol
			for inj_rate in "${inj_rate_values[@]}"
			do
				for i in {1..15}
				do
					echo $i
					./Project H=$h ncores=64 nchannels=4 inj_rate=$inj_rate mac_protocol=$mac_protocol sigma=$sigma >> $LOG_DIR
				done
			done
		done
	done
done
