#!/usr/bin/python
# -*- coding: UTF-8 -*-
import random
import argparse
import csv
parser = argparse.ArgumentParser()
parser.add_argument("indexed", nargs = '*')
args = parser.parse_args()

for txt_file in args.indexed:
    txt_input = open(txt_file, 'r')
    try:
        lines = txt_input.readlines()
    finally:
        txt_input.close()

    distribution_dic = {}

    for line in lines:
        line = line.strip()
        space_split_list = line.split(' ')
        space_counts = len(space_split_list)
        if space_counts not in distribution_dic:
            distribution_dic[space_counts] = 1
        else:
            distribution_dic[space_counts] += 1

    max_size = max(distribution_dic)

    total_size = 0
    number_of_sets = 0
    for key in distribution_dic:
        total_size += int(key) * int(distribution_dic[key])
        number_of_sets += int(distribution_dic[key])
    avg_size = total_size / float(number_of_sets)

    csv_name = txt_file[:-4] + '-distribution.csv'
    csv_file = open(csv_name, 'w')
    writer = csv.writer(csv_file)
    writer.writerow(['set size (number of token)', 'number of set'])
    for key in distribution_dic:
        writer.writerow([key, distribution_dic[key]])
    writer.writerow(['',''])
    writer.writerow(['overall number of sets', number_of_sets])
    writer.writerow(['max set size', max_size])
    writer.writerow(['average set size', avg_size])
    csv_file.close()
