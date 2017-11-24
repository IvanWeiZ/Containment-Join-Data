#!/usr/bin/python
# -*- coding: UTF-8 -*-
import random
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("indexed")
parser.add_argument("indexedout")
parser.add_argument("--propotion", type = float)
parser.add_argument("--number", type = int)
args = parser.parse_args()

txt_input = open(args.indexed, 'r')
try:
    lines = txt_input.readlines()
finally:
    txt_input.close()

len_input = len(lines)
if args.propotion:
    if args.propotion > 1:
        print 'Sample propotion over the limits. Please re-enter --propotion.'
        exit()
    len_sample = int(round(len_input * args.propotion))
    if args.propotion == 0.5:
        sample_name = args.indexedout + '50-1.txt'
        join_name =  args.indexedout + '50-2.txt'
    else:
        sample_name = args.indexedout + str(int(args.propotion * 100)) + '.txt'
        join_name =  args.indexedout + str(100 - int(args.propotion * 100)) + '.txt'
elif args.number:
    if args.number > len_input:
        print 'Sample length over the limits. Please re-enter --number.'
        exit()
    len_sample = args.number
    sample_name = args.indexedout + '-sample' + str(args.number) + '.txt'
    join_name =  args.indexedout + '-join.txt'
else:
    print 'Please enter --propotion 0.x or --number xx.'
    exit()

sample_index = random.sample(range(len_input), len_sample)
sample_index.sort()


sample_txt = open(sample_name, 'w')
join_txt = open(join_name, 'w')
for index, line in enumerate(lines):
    if sample_index:
        if index == sample_index[0]:
            sample_txt.write(line)
            del sample_index[0]
        else:
            join_txt.write(line)
    else:
        join_txt.write(line)
sample_txt.close()
join_txt.close()
