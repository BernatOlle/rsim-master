#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
from scipy import stats as scistats


def first(path, word):
    data_path = path + "/results/data_" + word + "/"

    output_file = path + '/results/first_spreadsheet_' + word + '.csv'
    try:
        os.remove(output_file)
    except OSError:
        pass
    fo = open(output_file, 'a')
    first_time = True
    for (root, dirs, files) in os.walk(data_path, topdown=True):
        dirs.sort()
        files.sort(key=float)
        if not files:
            first_time = True
        if files and first_time == True:
            # fo.write('\n')
            fo.write(' ;')
            for name in files:
                print("FILENAME first time : ", name)
                fo.write(name)
                fo.write(';')
            fo.write('\n')
            first_time = False
        fo.write(root.rpartition('/')[2])
        fo.write(';')
        for name in files:
            print("FILENAME : ", name)
            print(os.path.join(root, name))
            text_file = open(os.path.join(root, name), "r")
            my_data = [float(line.rstrip('\n')) for line in text_file]
            text_file.close()
            print(list(my_data))
            print(scistats.gmean(my_data))
            fo.write('%s' % scistats.gmean(my_data))
            fo.write(';')
        fo.write('\n')
        # for name in dirs:
        # print("PATH JOIN : ", os.path.join(root, name))
        # print("DIRNAME : ", name)
        # print('------------------')

    fo.close()

    return output_file


def second(file, word):
    output_file = path + '/results/second_spreadsheet_' + word + '.csv'

    fo = open(file, 'r')
    fa = open(output_file, 'a')

    for line in fo:
        if line.startswith(" ;"):
            fa.write("injection rates")
            fa.write(line)
        else:
            fa.write(line)

    return output_file


def third(file, word):
    output_file = path + '/results/final_spreadsheet_' + word + '.csv'

    fo = open(file, 'r')
    fa = open(output_file, 'a')

    for line in fo:
        if ("brs" in line) or ("token" in line):
            fa.write(word)
            fa.write(";")
            fa.write(line)
            fa.write("\n")
        else:
            fa.write(line)


if __name__ == '__main__':
    path = os.path.dirname(os.path.realpath(__file__))
    print("PATH : ", path)

    first_Latencies = first(path, "Latencies")
    print("csv created from all the Latencies data")
    # first_Latencies = path + "/results/first_spreadsheet_Latencies.csv"
    first_Throughput = first(path, "Throughput")
    print("csv created from all the Throughput data")
    # first_Throughput = path + "/results/first_spreadsheet_Throughput.csv"

    second_Latencies = second(first_Latencies, "Latencies")
    print("csv created from edited Latencies data")
    # second_Latencies = path + "/results/second_spreadsheet_Latencies.csv"
    second_Throughput = second(first_Throughput, "Throughput")
    print("csv created from edited Throughput data")
    # second_Throughput = path + "/results/second_spreadsheet_Throughput.csv"

    third(second_Latencies, "Latencies")
    print("final csv created from edited Latencies data again")
    # third_Latencies = path + "/results/second_spreadsheet_Latencies.csv"
    third(second_Throughput, "Throughput")
    print("final csv created from edited Throughput data again")
    # third_Throughput = path + "/results/second_spreadsheet_Throughput.csv"

    os.remove(first_Throughput)
    os.remove(first_Latencies)
    os.remove(second_Throughput)
    os.remove(second_Latencies)
