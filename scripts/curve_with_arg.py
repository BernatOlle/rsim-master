import csv
import os
import matplotlib.pyplot as plt
from pathlib import Path
import sys
from numpy import arange
from pandas import read_csv
from scipy.optimize import curve_fit


# define the true objective function
def objective(x, a, b, c):
	return a * x + b * x**2 + c
	
path = os.path.dirname(os.path.realpath(__file__))

name=sys.argv[1]

print("PATH : ", path)

save_path = path +"/curve_and_fit/"+ name + ".png"
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
				throughput_brs = [float(str(elm)) for elm in row]
			if "fuzzy_token" in row:
				row = row[2:len(row)-1]
				throughput_fuzzy = [float(str(elm)) for elm in row]
			if "token" in row:
				row = row[2:len(row)-1]
				throughput_token = [float(str(elm)) for elm in row]
		if "Latencies" in row:
			if "brs_non_p" in row:
				row = row[2:len(row)-1]
				latency_brs = [float(str(elm)) for elm in row]
			if "fuzzy_token" in row:
				row = row[2:len(row)-1]
				latency_fuzzy = [float(str(elm)) for elm in row]
			if "token" in row:
				row = row[2:len(row)-1]
				latency_token = [float(str(elm)) for elm in row]
			

throughput_brs = throughput_brs[:3]
throughput_fuzzy = throughput_fuzzy[:3]
throughput_token= throughput_token[:3]
latency_brs = latency_brs[:3]
latency_fuzzy = latency_fuzzy[:3]
latency_token = latency_token[:3]

f = open("results_functions.csv", "a+")
fo = open("axes_results.csv", "a+")
f.write('\n')
f.write(name)
f.write('\n')
fo.write('\n')
fo.write(name)
fo.write('\n')
# curve fit
popt_brs, _ = curve_fit(objective, throughput_brs, latency_brs)
# summarize the parameter values
a_brs, b_brs, c_brs = popt_brs
f.write('BRS : y = %.3f * x + %.3f * x^2 + %.3f' % (a_brs, b_brs, c_brs))
f.write("\n")
fo.write("x brs:")
for elm in throughput_brs:
	fo.write("\n")
	fo.write(str(elm))
fo.write('\n')
fo.write("y_brs:")
for elm in latency_brs:
	fo.write("\n")
	fo.write(str(elm))
fo.write("\n")
print('BRS : y = %.3f * x + %.3f * x^2 + %.3f' % (a_brs, b_brs, c_brs))
plt.scatter(throughput_brs, latency_brs)
# define a sequence of inputs between the smallest and largest known inputs
x_line_brs = arange(min(throughput_brs), max(throughput_brs), 0.0003)
# calculate the output for the range
y_line_brs = objective(x_line_brs, a_brs, b_brs, c_brs)
# create a line plot for the mapping function
plt.plot(x_line_brs, y_line_brs, '--', color='pink')


# curve fit
popt_token, _ = curve_fit(objective, throughput_token, latency_token)
# summarize the parameter values
a_token, b_token, c_token = popt_token
f.write('TOKEN : y = %.3f * x + %.3f * x^2 + %.3f' % (a_token, b_token, c_token))
f.write("\n")
fo.write("x token:")
for elm in throughput_token:
	fo.write("\n")
	fo.write(str(elm))
fo.write('\n')
fo.write("y_token:")
for elm in latency_token:
	fo.write("\n")
	fo.write(str(elm))
fo.write("\n")
print('TOKEN : y = %.3f * x + %.3f * x^2 + %.3f' % (a_token, b_token, c_token))
plt.scatter(throughput_token, latency_token)
# define a sequence of inputs between the smallest and largest known inputs
x_line_token = arange(min(throughput_token), max(throughput_token), 0.0003)
# calculate the output for the range
y_line_token = objective(x_line_token, a_token, b_token, c_token)
# create a line plot for the mapping function
plt.plot(x_line_token, y_line_token, ':', color='brown')

# curve fit
popt_fuzzy, _ = curve_fit(objective, throughput_fuzzy, latency_fuzzy)
# summarize the parameter values
a_fuzzy, b_fuzzy, c_fuzzy = popt_fuzzy
f.write('FUZZY : y = %.3f * x + %.3f * x^2 + %.3f' % (a_fuzzy, b_fuzzy, c_fuzzy))
f.write("\n")
fo.write("x fuzzy:")
for elm in throughput_fuzzy:
	fo.write("\n")
	fo.write(str(elm))
fo.write('\n')
fo.write("y_fuzzy:")
for elm in latency_fuzzy:
	fo.write("\n")
	fo.write(str(elm))
fo.write("\n")
print('FUZZY : y = %.3f * x + %.3f * x^2 + %.3f' % (a_fuzzy, b_fuzzy, c_fuzzy))
plt.scatter(throughput_fuzzy, latency_fuzzy)
# define a sequence of inputs between the smallest and largest known inputs
x_line_fuzzy = arange(min(throughput_fuzzy), max(throughput_fuzzy), 0.0003)
# calculate the output for the range
y_line_fuzzy = objective(x_line_fuzzy, a_fuzzy, b_fuzzy, c_fuzzy)
# create a line plot for the mapping function
plt.plot(x_line_fuzzy, y_line_fuzzy, '--', color='red')







plt.savefig(save_path)
		

