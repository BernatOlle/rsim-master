import csv
import os
import matplotlib.pyplot as plt
from pathlib import Path
import sys

def create_axes(path, name):

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
				if "fuzzy_token" in row:
					row = row[2:len(row)-1]
					throughput_fuzzy = [float(elm) for elm in row]
				if "token" in row:
					row = row[2:len(row)-1]
					throughput_token = [float(elm) for elm in row]
			if "Latencies" in row:
				if "brs_non_p" in row:
					row = row[2:len(row)-1]
					latency_brs = [float(elm) for elm in row]
				if "fuzzy_token" in row:
					row = row[2:len(row)-1]
					latency_fuzzy = [float(elm) for elm in row]
				if "token" in row:
					row = row[2:len(row)-1]
					latency_token = [float(elm) for elm in row]
	return [throughput_brs, latency_brs, throughput_fuzzy, latency_fuzzy, throughput_token, latency_token]

if __name__=="__main__": 

	path = os.path.dirname(os.path.realpath(__file__))

	name1=sys.argv[1]
	name2=sys.argv[2]
	name3=sys.argv[3]
	name4=sys.argv[4]
	name5=sys.argv[5]
	name6=sys.argv[6]
	name7=sys.argv[7]
	name8=sys.argv[8]
	name9=sys.argv[9]

	print("PATH : ", path)

	save_path = path + "/comparative_graph_N256.png"
	path1 = path + "/excel_data/" + name1 + ".csv"
	path2 = path + "/excel_data/" + name2 + ".csv"
	path3 = path + "/excel_data/" + name3 + ".csv"
	path4 = path + "/excel_data/" + name4 + ".csv"
	path5 = path + "/excel_data/" + name5 + ".csv"
	path6 = path + "/excel_data/" + name6 + ".csv"
	path7 = path + "/excel_data/" + name7 + ".csv"
	path8 = path + "/excel_data/" + name8 + ".csv"
	path9 = path + "/excel_data/" + name9 + ".csv"
	
	result_axes1 = create_axes(path1, name1)
	result_axes2 = create_axes(path2, name2)
	result_axes3 = create_axes(path3, name3)
	result_axes4 = create_axes(path4, name4)
	result_axes5 = create_axes(path5, name5)
	result_axes6 = create_axes(path6, name6)
	result_axes7 = create_axes(path7, name7)
	result_axes8 = create_axes(path8, name8)
	result_axes9 = create_axes(path9, name9)
	
	fig, ((ax1,ax2,ax3),(ax4,ax5,ax6),(ax7,ax8,ax9)) = plt.subplots(3, 3)
	fig.suptitle("N256") 
	fig.supxlabel("sigma values")
	fig.supylabel("hurst values")
	ax1.plot(result_axes1[0],result_axes1[1],marker='o',label='brs non p')
	ax1.plot(result_axes1[2],result_axes1[3],marker='o',label='fuzzy token')
	ax1.plot(result_axes1[4],result_axes1[5],marker='o',label='token')
	ax1.set_ylim(0, 1000)
	ax1.legend()
	ax1.set_ylabel("H 0.5")
	ax2.plot(result_axes2[0],result_axes2[1],marker='o',label='brs non p')
	ax2.plot(result_axes2[2],result_axes2[3],marker='o',label='fuzzy token')
	ax2.plot(result_axes2[4],result_axes2[5],marker='o',label='token')
	ax2.set_ylim(0, 1000)
	ax3.plot(result_axes3[0],result_axes3[1],marker='o',label='brs non p')
	ax3.plot(result_axes3[2],result_axes3[3],marker='o',label='fuzzy token')
	ax3.plot(result_axes3[4],result_axes3[5],marker='o',label='token')
	ax3.set_ylim(0, 1000)
	ax4.plot(result_axes4[0],result_axes4[1],marker='o',label='brs non p')
	ax4.plot(result_axes4[2],result_axes4[3],marker='o',label='fuzzy token')
	ax4.plot(result_axes4[4],result_axes4[5],marker='o',label='token')
	ax4.set_ylim(0, 5000)
	ax4.set_ylabel("H 0.7")
	ax5.plot(result_axes5[0],result_axes5[1],marker='o',label='brs non p')
	ax5.plot(result_axes5[2],result_axes5[3],marker='o',label='fuzzy token')
	ax5.plot(result_axes5[4],result_axes5[5],marker='o',label='token')
	ax5.set_ylim(0, 5000)
	ax6.plot(result_axes6[0],result_axes6[1],marker='o',label='brs non p')
	ax6.plot(result_axes6[2],result_axes6[3],marker='o',label='fuzzy token')
	ax6.plot(result_axes6[4],result_axes6[5],marker='o',label='token')
	ax6.set_ylim(0, 000)
	ax7.plot(result_axes7[0],result_axes7[1],marker='o',label='brs non p')
	ax7.plot(result_axes7[2],result_axes7[3],marker='o',label='fuzzy token')
	ax7.plot(result_axes7[4],result_axes7[5],marker='o',label='token')
	ax7.set_ylim(0, 25000)
	ax7.set_ylabel("H 0.9")
	ax7.set_xlabel("S 0.5")
	ax8.plot(result_axes8[0],result_axes8[1],marker='o',label='brs non p')
	ax8.plot(result_axes8[2],result_axes8[3],marker='o',label='fuzzy token')
	ax8.plot(result_axes8[4],result_axes8[5],marker='o',label='token')
	ax8.set_ylim(0, 25000)
	ax8.set_xlabel("S 10")
	ax9.plot(result_axes9[0],result_axes9[1],marker='o',label='brs non p')
	ax9.plot(result_axes9[2],result_axes9[3],marker='o',label='fuzzy token')
	ax9.plot(result_axes9[4],result_axes9[5],marker='o',label='token')
	ax9.set_ylim(0, 25000)
	ax9.set_xlabel("S 100")
	
	plt.show()
	plt.savefig(save_path)
		

