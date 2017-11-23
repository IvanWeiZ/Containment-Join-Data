import csv
import sys
#with open(sys.argv[1], newline='') as csvfile:
with open("output.csv", newline='') as csvfile:
	reader = csv.DictReader(csvfile)
	for row in reader:
		print(row)
		# for ele in row:
		# 	print(ele)
		# mydict = {ele[0]:ele[1] for ele in row}
		# print(mydict)