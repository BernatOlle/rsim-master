#!/bin/bash

mac_protocols=(
brs_non_p
token
fuzzy_token
)

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
# 0.0002
# 0.0003
# 0.0004
# 0.0005
# 0.0006
# 0.0007
# 0.0008
# 0.0009
0.0010
# 0.0011
0.0012
# 0.0013
# 0.0014
0.0015
# 0.0016
0.0017
# 0.0018
0.0019
# 0.0020
0.0021
# 0.0022
0.0023
# 0.0024
# 0.0025
0.0026
# 0.0027
# 0.0028
# 0.0029
0.0030
# 0.0031
# 0.0032
# 0.0033
# 0.0034
0.0035
# 0.0036
# 0.0037
# 0.0038
# 0.0039
0.0040
0.0045
# 0.0050
# 0.0055
# 0.0060
# 0.0065
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
				./Project ncores=64 inj_rate=$inj_rate tx_time=4 npackets=10000 mac_protocol=$mac_protocol sigma=$sigma inj_distribution=poisson &
			done
            wait
		done
	done
done
