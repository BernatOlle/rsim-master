#!/usr/bin/python
# -*- coding: utf-8 -*-
from numpy import genfromtxt
from scipy import stats as scistats
import os
import sys
import getopt


def main(argv):
    resultsfolder = ''
    try:
        (opts, args) = getopt.getopt(argv, 'hr:', ['realappsfolder='])
    except getopt.GetoptError:
        print 'parse_result.py -r <realappsfolder>'
        sys.exit(2)
    for (opt, arg) in opts:
        if opt == '-h':
            print 'parse_result.py -r <realappsfolder>'
            sys.exit()
        elif opt in ('-r', '--realapps-folder'):
            resultsfolder = arg
    print 'Realapps results folder is ' + resultsfolder

    output_file = resultsfolder + 'aggregated_spreadsheet_latencies.csv'
    try:
        os.remove(output_file)
    except OSError:
        pass
    fo = open(output_file, 'a')
    first_time = True
    for (root, dirs, files) in os.walk(yourpath, topdown=True):
        dirs.sort()
        files.sort(key=float)
        if not files:
            first_time = True
        if files and first_time == True:
            for name in files:
                fo.write(name)
                fo.write(';')
            fo.write('\n')
            first_time = False
        for name in files:
            print os.path.join(root, name)
            text_file = open(os.path.join(root, name), "r")
            my_data = [float(line.rstrip('\n')) for line in text_file]
            text_file.close()
            print (list(my_data))
            print scistats.gmean(my_data)
            fo.write('%s' % scistats.gmean(my_data))
            fo.write(';')
        fo.write(root)
        fo.write('\n')
        for name in dirs:
            print os.path.join(root, name)
        print '------------------'

    fo.close()


if __name__ == '__main__':
    main(sys.argv[1:])
