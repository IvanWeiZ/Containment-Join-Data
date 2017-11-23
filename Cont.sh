#!/bin/bash

inputfile=$1
inputname=$2

function writeInfo {
	echo "-------------------------------------------------------------------------" >> "$6"
	echo "$1" >> "$6"
	echo "$2" >> "$6"
	echo "$3" >> "$6"
	echo "$4" >> "$6"
	echo "$5" >> "$6"
}  

function writeInfo1 {
	#echo "------------" >> "$6"
	echo "-------------------------------------------------------------------------"
	echo "$1"
	echo "$2"
	echo "$3"
	echo "$4"
	echo "$5"
}  

mkdir "./output/""$inputname"

#declare -a arr=("jaccard" "cosine" "dice" "containment")
declare -a arr=("containment")
declare -a excutables=( "set_sim_join_candonly"  "set_sim_join_ext_stat" "set_sim_join_nostat")
declare -a functions=("ppjoin" "mpjoin" "mpjoin_PEL" "groupjoin" "allpairs" "adaptjoin" "PPjoin+")
# "set_sim_join_cycles" "set_sim_join"
for threshold in 0.95 0.9 0.85 0.8 0.75 0.7 0.65 0.6 0.55 0.5
do
	for i in "${excutables[@]}" 
	do
		for j in "${arr[@]}"
		do
			for f in "${functions[@]}"
			do
			function_name="$f"

			exeLoc="/u/zhengw14/CSC2525-Project/ssjoin-0.1/"

			sim="$j"

			if [ "$j" == "containment" ]
			then
				exeLoc="/u/zhengw14/CSC2525-Cont/CSC2525-Project/ssjoin-0.1/"
				sim="dice"
			fi

			output="./output/""$inputname""/""$inputname"-"$function_name"-"$j"-"$threshold"-"$i"

			writeInfo "$inputfile" "$function_name" "$j" "$threshold" "$i" "$output"

			if [ "$f" == "mpjoin" ];
			then 
				echo "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "ppjoin"  --timings --statistics  \
				--input "$inputfile" --mpjoin

				time "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "ppjoin"  --timings --statistics  \
				--input "$inputfile" --mpjoin >> "$output"
			elif [ "$f" == "mpjoin_PEL" ];
				then
				echo "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "ppjoin"  --timings --statistics  \
				--input "$inputfile"  --pljoin --mpjoin

				time "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "ppjoin"  --timings --statistics  \
				--input "$inputfile"  --pljoin --mpjoin >> "$output"


			elif [ "$f" == "PPjoin+" ];
				then
				echo "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "ppjoin"  --timings --statistics  \
				--input "$inputfile" --suffixfilter

				time "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "ppjoin"  --timings --statistics  \
				--input "$inputfile" --suffixfilter >> "$output"
				
			else
				echo "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "$f"  --timings --statistics  \
				--input "$inputfile" 

				time "$exeLoc""$i" --threshold "$threshold" --"$sim"  \
				--algorithm "$f"  --timings --statistics  \
				--input "$inputfile" >> "$output"
			fi
			done

		done	  
	done
done

exit 0


