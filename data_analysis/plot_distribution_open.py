#!/usr/bin/python
# -*- coding: UTF-8 -*-
import random
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("indexed", nargs = '*')
args = parser.parse_args()

import matplotlib.pyplot as plt
import numpy as np
fig = plt.figure()
ax = fig.add_subplot(111)

import csv
for counter, csv_name in enumerate(args.indexed):
    csv_file = open(csv_name, 'r')
    csv_reader = csv.reader(csv_file)
    rows = [row for row in csv_reader]
    del rows[0]
    del rows[-4:]
    csv_file.close()
    
    count = []
    tokens = []
    for row in rows:
        count.append(int(row[0]))
        tokens.append(int(row[1]))

    plot_name = csv_name[:-4]
    #xaxis = np.log(np.array(tokens))
    plot_name = ax.scatter(tokens, count,
                           marker='o',
                           label = 'Open Canadian Data',
                           color = 'tab:blue')

axes = plt.gca()
#axes.set_ylim([0,10000])
#axes.set_xlim([0,2])
plt.xlabel('token size')
plt.ylabel('count')
plt.legend(loc=2)
fig.savefig('dis_opendata.png') 
plt.close(fig)
