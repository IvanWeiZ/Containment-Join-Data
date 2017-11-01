#!/usr/bin/python

import numpy.random
import random
import sys

# Note: This script could be more memory-efficient
# Probably, twice the output size is the memory requirement

sets = int(sys.argv[1])
avgsetsize = int(sys.argv[2])
universe = int(sys.argv[3])
zipfparam = float(sys.argv[4])

def getsetsize(setsizeavg):
	return numpy.random.poisson(setsizeavg)

def harmon_number(n, a):
	su = 0
	for i in range(n,0,-1):
		su += 1.0/(i+0.0)**a
	return su

probtoken = []

lastval = 0
hnum = harmon_number(universe, zipfparam)

for i in range(1,universe+1):
	val = 1/(i**zipfparam * hnum)
	lastval += val
	probtoken.append(val)

tokensneeded = 0
setsizelist = []
for i in range(sets):
	setsize = getsetsize(avgsetsize)
	tokensneeded += setsize
	if setsize != 0:
		setsizelist.append(setsize)

tokens=[]

for i, p in enumerate(probtoken,1):
	tokeni = int(round(p * tokensneeded))
	tokens.extend([i]*tokeni)

random.shuffle(tokens)

begin = 0
for ss in setsizelist:
	print " ".join([ str(v) for v in tokens[begin:begin+ss] ])
	begin += ss
