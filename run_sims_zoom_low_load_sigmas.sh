#!/bin/bash

mac_protocols=(
brs_non_p
token
fuzzy_token
)

sigma_values=(
# 0.05
0.1
0.2
0.3
0.4
0.5
0.6
0.7
0.8
0.9
1
10
100
)

inj_rate_values=(
# 0.0001
# 0.0002
# 0.0003
# 0.0004
# 0.0005
# 0.0006
0.0007
# 0.0008
# 0.0009
# 0.0010
# 0.0011
# 0.0012
# 0.0013
# 0.0014
# 0.0015
# 0.0016
0.0017
# 0.0018
# 0.0019
# 0.0020
# 0.0021
# 0.0022
# 0.0023
# 0.0024
# 0.0025
# 0.0026
# 0.0027
# 0.0028
# 0.0029
# 0.0030
# 0.0031
# 0.0032
# 0.0033
# 0.0034
# 0.0035
# 0.0036
# 0.0037
# 0.0038
# 0.0039
# 0.0040
# 0.0045
# 0.0050
# 0.0055
# 0.0060
# 0.0065
)


LOG_DIR=./output/output_sigmas_repeat
RESULTS_DIR=./results/results_sigmas_repeat/data_throughput/
RESULTS_DIR2=./results/results_sigmas_repeat/data_latencies/

rm $RESULTS_DIR -r
rm $RESULTS_DIR2 -r
rm $LOG_DIR -r


for sigma in "${sigma_values[@]}"
do
	for mac_protocol in "${mac_protocols[@]}"
	do
		echo $mac_protocol
		for inj_rate in "${inj_rate_values[@]}"
		do
			echo injection rate: $inj_rate
			for i in {1..10}
			do
                mkdir -p $LOG_DIR/
				echo iteration: $i
				# ./Project ncores=64 inj_rate=$inj_rate mac_protocol=$mac_protocol >> $LOG_DIR
				./Project ncores=64 inj_rate=$inj_rate tx_time=4 npackets=10000 mac_protocol=$mac_protocol inj_distribution=poisson sigma=$sigma save_results_path=./results/results_sigmas_repeat  &> ${LOG_DIR}/${sigma}_${inj_rate}_${mac_protocol}_${i}.txt &
			done
			wait
		done
	done
done
