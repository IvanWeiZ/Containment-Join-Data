#!/usr/bin/python
# -*- coding: utf-8 -*-
import csv
import re
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("indexed")
parser.add_argument("indexedoutput")
args = parser.parse_args()

# Read csv file by column
csv_file = open(args.indexed, 'rb')
columns = []

try:
    csv_reader = csv.reader(csv_file)
    rows= [row for row in csv_reader]
    del rows[0]
    for i in range(len(rows[0])):
        columns.append([row[i].replace(' ','') for row in rows])
except:
    print 'Error reading csv file in preprocessing.'
finally:
    csv_file.close()

# Write columns to txt file
txt_file = open(args.indexedoutput, 'w')

punctuations = '[’!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~]+'

try:
    for column in columns:
        for token in column:
            # Remove fractional part if .00
            token = token.replace('.00', '')
            # Remove punctuations
            token = re.sub(punctuations, '', token)
            txt_file.write(''.join([token + ' ']))
        txt_file.write(''.join([token + '\n']))
except:
    print 'Error writing txt file in preprocessing.'
finally:
    txt_file.close( )

