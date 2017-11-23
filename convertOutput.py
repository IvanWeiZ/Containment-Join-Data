import sys
import csv
import os
import platform

def scrape(filename):
	file=open(filename, "r")
	print(filename)
	#lines=[for x ]
	lines=[ele.strip().split() for ele in file.readlines()]
	d={'filename':None,'algo':None,'sim':None,'threshold':None,'exe':None,'pairs':None,'lookups':None,
	'indexEntriesSeen':None,'candidatesP1':None,'candidatesVerify':None,'verifyTrueSteps':None,
	'verifyTrueSetSizeSum':None,'verifyTrueSetSizeCnt':None,'verifyFalseSteps':None,'verifyFalseSetSizeSum':None,
	'verifyFalseSetSizeCnt':None,'verifyLoop0True':None,'verifyLoop0False':None,'readrawrecords':None,'algoindex':None,
	'algojoin':None,'readrawforeignrecords':None,'adaptjoinlastext0':None,'adaptjoinlastext1':None,'adaptjoinlastext2':None,'adaptjoinlastext3':None,
	'adaptjoinlastext4':None,'adaptjoinlastext5':None,'adaptjoinlastext6':None,'adaptjoinlastext7':None}
	
	d["filename"]=lines[1][0]
	d["algo"]=lines[2][0]
	d["sim"]=lines[3][0]
	d["threshold"]=float(lines[4][0])
	d["exe"]=lines[5][0]
	d["pairs"]=int(lines[8][3])

	for i in range(9,len(lines)):
		if lines[i][0]=="Statistics:" or lines[i][0]=="Extended"  or lines[i][0]=="Timings:" :
			continue
		d[lines[i][0]]=int(lines[i][1])

	#print(d.values)
	#filename,algo,sim,threshold,exe,pairs,lookups,indexEntriesSeen,candidatesP1,candidatesVerify,verifyTrueSteps,verifyTrueSetSizeSum,verifyTrueSetSizeCnt,verifyFalseSteps,verifyFalseSetSizeSum,verifyFalseSetSizeCnt,verifyLoop0True,verifyLoop0False,readrawrecords,algoindex,algojoin,adaptjoinlastext0,adaptjoinlastext1,adaptjoinlastext2,adaptjoinlastext3,adaptjoinlastext4,adaptjoinlastext5,adaptjoinlastext6,adaptjoinlastext7
	with open('output.csv', 'a', newline='') as csvfile:
		fieldnames = ['filename','algo','sim','threshold','exe','pairs','lookups',
		'indexEntriesSeen','candidatesP1','candidatesVerify','verifyTrueSteps','verifyTrueSetSizeSum',
		'verifyTrueSetSizeCnt','verifyFalseSteps','verifyFalseSetSizeSum','verifyFalseSetSizeCnt',
		'verifyLoop0True','verifyLoop0False','readrawrecords','algoindex','algojoin','readrawforeignrecords','adaptjoinlastext0',
		'adaptjoinlastext1','adaptjoinlastext2','adaptjoinlastext3','adaptjoinlastext4','adaptjoinlastext5',
		'adaptjoinlastext6','adaptjoinlastext7','adaptjoinlastext8','adaptjoinlastext9','adaptjoinlastext10',
		'adaptjoinlastext11','adaptjoinlastext12','adaptjoinlastext13','adaptjoinlastext14','adaptjoinlastext15',
		'adaptjoinlastext16','adaptjoinlastext17','adaptjoinlastext18','adaptjoinlastext19','adaptjoinlastext20']
		
		writer = csv.DictWriter(csvfile,fieldnames=fieldnames)
		if os.stat('output.csv').st_size==0:
			writer.writeheader()
		writer.writerow(d)

	#print(d.keys)




if __name__ == '__main__':
    for i in range(1,len(sys.argv)):
        scrape(sys.argv[i])