import os 
##import matplotlib.pyplot as plt
from pathlib import Path
##import numpy as np
import collections
from scipy import stats as scistats
	
	
def parse_data(path, sub_folder):
	
	output_file = path + "/results/spreadsheet_results_"+sub_folder+".csv"
	#os.remove(output_file)
	output_file = path + "/results/spreadsheet_results_"+sub_folder+".csv"
	path = path + "/results/data_"+sub_folder+"/"
	
	part = ""

	fo = open(output_file, 'a')
	first_time = True
	print(sub_folder)
	fo.write(sub_folder)
	for (dirpath, dirnames, filenames) in os.walk(path, topdown = True):
		dirnames.sort()
		filenames.sort(key=float)
		if not filenames:
			first_time = True
		if filenames and first_time == True:
			for name in dirnames:
				fo.write(name)
				fo.write("\n")
			for name in filenames:
				print("NAME OF FILENAMES : ", name)
				print("------")
				fo.write(";")
				fo.write(name)
				fo.write(";")
			fo.write("\n")
			first_time = False
		fo.write(dirpath.partition(sub_folder+"/"+ part)[-1])
		for name in filenames:
			print(os.path.join(dirpath, name))
			text_file = open(os.path.join(dirpath, name), "r")
			my_data = [float(line.rstrip("\n")) for line in text_file]
			text_file.close()
			print((list(my_data)))
			fo.write(";")
			fo.write('%s' % scistats.gmean(my_data))
			fo.write(';')
		print	("DIRPATH : ", dirpath)
		fo.write("\n")
		for name in dirnames:
			print("NAME OF DIRNAMES : ", name)
			print("DIRNAME : ", os.path.join(dirpath, name))
		print("--------------------------------------------------------------------")
	

if __name__=="__main__": 
	path = os.path.dirname(os.path.realpath(__file__))
	
	parse_data(path, "throughput")
	parse_data(path, "latencies")
	
	csv_throughputs = parse_data(path, "throughput")
	csv_latencies = parse_data(path, "latencies")
	#plot(csv_throughputs, csv_latencies)
