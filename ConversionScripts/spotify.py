#!/usr/bin/python

import re
import sys


def artist(artist, title):
	return artist

def track(artist,title):
	return "%s,%s" % (artist, title)

inp = sys.argv[1]
strat = track
if len(sys.argv) > 2 and sys.argv[2] == "artist":
	strat = artist


users = {}

item2keys = {}
nextkey = 1

with open(inp) as istr:
	for line in istr:
		splitres = line.split(",")
		if not splitres[0] in users:
			users[splitres[0]] = []
		item = strat(splitres[2], splitres[1])
		if item not in item2keys:
			item2keys[item] = nextkey
			nextkey += 1
		users[splitres[0]].append(item2keys[item])

for user, items  in users.iteritems():
	print " ".join([str(it) for it in items])
