#!/usr/bin/python
# -*- coding: UTF-8 -*-
import random
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("indexed", nargs = '*')
args = parser.parse_args()

import matplotlib.pyplot as plt
plot_colors = ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 'tab:purple', 'tab:brown', 'tab:pink', 'tab:gray']
text = iter(['BMS',
             'DBLP',
             'ENRON',
             'KOSARAK',
             'LIVEJOURNAL',
             'ORKUT',
             'UNIFORM',
             'ZIPF'])
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
        count.append(row[0])
        tokens.append(row[1])

    plot_name = csv_name[:-4]
    plot_name = ax.scatter(tokens, count,
                            marker='o',
                            label=next(text),
                            color = plot_colors[counter])

axes = plt.gca()
axes.set_ylim([0,1000])
axes.set_xlim([0,50000])
plt.xlabel('token size')
plt.ylabel('count')
plt.legend(loc=2)
plt.show()
