#!/bin/bash

mac_protocols=(
brs_non_p
# tdma_fixed
#tdma_weighted
token
fuzzy_token
)

app_names=(
bfs
bodytrack
canneal
cc
community
fft
ocean-c
ocean-nc
pagerank
radiosity
raytrace
sssp
streamcluster
volrend
water
)

LOG_DIR=./output/output_realapps_repeat
RESULTS_DIR=./results/results_realapps

rm $RESULTS_DIR -r
rm $LOG_DIR -r

for app in "${app_names[@]}"
do
	echo $app
	for mac_protocol in "${mac_protocols[@]}"
	do
		echo $mac_protocol
		# if we use brs, since it's non-deterministic, we need to repeat experiments multiple times and then we'll do average
		if [ "$mac_protocol" = "brs_non_p" ]
		then
			iters=3
		else
			iters=1
		fi
		for i in $(seq 1 $iters)
		do
			echo iteration: $i
            mkdir -p ${LOG_DIR}/
			touch ${LOG_DIR}/${app}_${mac_protocol}_${i}.txt
            mkdir -p ${RESULTS_DIR}/${app}
			# ./Project ncores=64 inj_rate=$inj_rate mac_protocol=$mac_protocol >> $LOG_DIR
			./Project ncores=64 tx_time=4 mac_protocol=$mac_protocol inj_distribution=trace load_trace_path=./traces/traces_sergi_m2s/token/$app/trace_for_rsim.txt save_results_path=${RESULTS_DIR}/${app} &> ${LOG_DIR}/${app}_${mac_protocol}_${i}.txt &
		done
	done
	wait
done
