#!/bin/bash

mac_protocols=(
brs_non_p
#tdma_fixed
#tdma_weighted
#token
#fuzzy_token
)

assig=(
rando
gre
)

sigma_values=(
#0.5
10
#100
)

h_values=(
#0.5
#0.7
0.85
#0.9
)

inj_rate_values=(
0.00001
0.00002
0.00003
0.00004
0.00005
0.00006
0.00007
0.00008
0.00009
)

ncores_values=(
16
# 20
#32
# 36
# 50
#64
# 100
#128
# 150
# 200
# 250
# 300
# 350
# 400
# 450
# 500
#512
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
#1024
)


LOG_DIR=./output/log.txt
RESULTS_DIR=./results/data_throughput/
RESULTS_DIR2=./results/data_latencies/

#rm $RESULTS_DIR -r
#rm $RESULTS_DIR2 -r
#rm $LOG_DIR
#touch $LOG_DIR

for ncore in "${ncores_values[@]}"
do
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
						./Project H=$h ncores=$ncore nchannels=4 inj_rate=$inj_rate mac_protocol=$mac_protocol sigma=$sigma
					done
				done
			done
		done
	done
done
