import csv
import os
import matplotlib.pyplot as plt
from pathlib import Path
import sys

path = os.path.dirname(os.path.realpath(__file__))

name=sys.argv[1]

print("PATH : ", path)

save_path = path + "/figures/" + name + ".png"
path = path + "/excel_data/" + name + ".csv"


throughput_brs = []
throughput_fuzzy = []
throughput_token = []
latency_brs = []
latency_fuzzy = []
latency_token = []
print("----------------------")
print("FILE : ", name)
print("----------------------")
with open(path, "r") as csvfile:
	next(csvfile)
	data = csv.reader(csvfile)
	for row in data:
		if "Throughput" in row:
			if "brs_non_p" in row:
				row = row[2:len(row)-1]
				throughput_brs = [float(elm) for elm in row]
				print("throughput_brs : ", throughput_brs)
			if "fuzzy_token" in row:
				row = row[2:len(row)-1]
				throughput_fuzzy = [float(elm) for elm in row]
				print("throughput_fuzzy : ", throughput_fuzzy)
			if "token" in row:
				row = row[2:len(row)-1]
				throughput_token = [float(elm) for elm in row]
				print("throughput_token : ",throughput_token)
		if "Latencies" in row:
			print("ROW : ", row)
			if "brs_non_p" in row:
				row = row[2:len(row)-1]
				latency_brs = [float(elm) for elm in row]
				print("latency_brs : ",latency_brs)
			if "fuzzy_token" in row:
				row = row[2:len(row)-1]
				latency_fuzzy = [float(elm) for elm in row]
				print("latency_fuzzy : ", latency_fuzzy)
			if "token" in row:
				row = row[2:len(row)-1]
				latency_token = [float(elm) for elm in row]
				print("latency_token : ",latency_token)
plt.title(name)			
#plt.xlim(0,1)
#plt.ylim(0,100)
plt.plot(throughput_brs,latency_brs,marker='o',label='brs non p')
plt.plot(throughput_fuzzy,latency_fuzzy,marker='o',label='fuzzy token')
plt.plot(throughput_token,latency_token,marker='o',label='token')
plt.legend()
#plt.show()
print("save path : ", save_path)
plt.savefig(save_path)
		

