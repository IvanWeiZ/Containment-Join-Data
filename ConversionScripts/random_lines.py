#!/usr/bin/python

import sys
import random

number = int(sys.argv[1])
filename = sys.argv[2]

with open(filename) as f:
	lines = f.readlines()

linestoprint = random.sample(xrange(len(lines)), number)

for ln in linestoprint:
	print lines[ln],
