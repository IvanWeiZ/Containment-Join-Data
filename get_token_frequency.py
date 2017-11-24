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

    token_dic = {}
    for line in lines:
        line = line.strip()
        space_split__token_list = line.split(' ')
        for token in space_split__token_list:
            if token not in token_dic:
                token_dic[token] = 1
            else:
                token_dic[token] += 1

    token_frequency_dic = {}
    unique_token_number = 0
    for token in token_dic:
        unique_token_number += 1
        key = int(token_dic[token])
        if key not in token_frequency_dic:
            token_frequency_dic[key] = 1
        else:
            token_frequency_dic[key] += 1

    sorted_token_frequency_dic = sorted(token_frequency_dic.items(), key=lambda item: item[0])

    csv_name = txt_file[:-4] + '-token-frequency.csv'
    csv_file = open(csv_name, 'w')
    writer = csv.writer(csv_file)
    writer.writerow(['token frequency (number of occurance)', 'count'])
    for item in sorted_token_frequency_dic:
        writer.writerow([item[0], item[1]])
    writer.writerow(['',''])
    writer.writerow(['overall number of unique tokens', unique_token_number])
    csv_file.close()
