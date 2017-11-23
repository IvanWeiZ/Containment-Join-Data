import sys
file1 = sys.argv[1]
file2 = sys.argv[2]

print(file1)
print(file2)

threshold=[0.5, 0.55 , 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95]
containment_Q=[0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
containment_X=[0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
count=0
with open(file1) as finput:
	with open(file2) as foreign:
		lines=[line.split() for line in finput.readlines()]
		flines=[line.split() for line in foreign.readlines()]
		for fl in flines:
			print(count)
			count+=1
     
			for l in lines:
				inter=len(set(fl).intersection(l))
				containment1=inter/len(fl)
				containment2=inter/len(l)
				for i in range(10):
					if containment1>=threshold[i]:
						containment_Q[i]+=1
					if containment2>=threshold[i]:
						containment_X[i]+=1


print(containment_Q)
print(containment_X)

