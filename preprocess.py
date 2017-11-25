#!/usr/bin/python
# -*- coding: utf-8 -*-
import csv
import sys
import re
import argparse
import os
import platform

def process(indexed):

    # parser = argparse.ArgumentParser()
    # parser.add_argument("indexed")
    # #parser.add_argument("indexedoutput")
    # args = parser.parse_args()

    filenames=indexed.split("/")
    outfilename="./procOutputTables/"+filenames[len(filenames)-2]+filenames[len(filenames)-1]

    print(outfilename)


    # Read csv file by column
    csv_file = open(indexed, 'rt')

    csv_reader = csv.reader(csv_file)
    rows= [row for row in csv_reader]

    if rows == []:
    	print('Empty Table in %s CSV File.\nSkip.' % indexed)
    	sys.exit()

    if len(rows) == 1:
    	print('One Row Table Body in %s CSV File (Only Table Title).\nSkip.' % indexed)
    	#sys.exit()

    # del rows[0]

    max_column = max(list(map(lambda row: len(row), rows)))

    columns = []
    for i in range(max_column):
        column = []
        for row in rows:
            if i < len(row):
                column.append(row[i].replace(' ',''))
        columns.append(column)

    csv_file.close()

    # Write columns to txt file
    txt_file = open(outfilename, 'w')

    punctuations = '[’!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~]+'
    for column in columns:
        for token in column:
            # Remove fractional part if .00
            token = token.replace('.00', '')
            # Remove punctuations
            token = re.sub(punctuations, '', token)
            txt_file.write(''.join([token + ' ']))
        txt_file.write(''.join(['\n']))

    txt_file.close( )


if __name__ == '__main__':
    for i in range(1,len(sys.argv)):
        process(sys.argv[i])
