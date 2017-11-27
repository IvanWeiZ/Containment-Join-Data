#!/bin/bash



/u/zhengw14/CSC2525-Cont/CSC2525-Project/ssjoin-0.1/set_sim_join_candonly --threshold 0.95 --cosine --algorithm adaptjoin --timings --statistics --foreign-linewise --foreign-input /u/zhengw14/canada_us_uk.ssjoin-sample10000.txt --input /u/zhengw14/canada_us_uk.ssjoin-join.txt

/u/zhengw14/CSC2525-Contx/CSC2525-Project/ssjoin-0.1/set_sim_join_ext_stat --threshold 0.6 --dice --algorithm ppjoin --timings --statistics --foreign-linewise --foreign-input /u/zhengw14/canada_us_uk.ssjoin-sample10000.txt --input /u/zhengw14/canada_us_uk.ssjoin-join.txt

/u/zhengw14/CSC2525-Cont/CSC2525-Project/ssjoin-0.1/set_sim_join_ext_stat --threshold 0.6 --cosine --algorithm ppjoin --timings --statistics --foreign-linewise --foreign-input /u/zhengw14/canada_us_uk.ssjoin-sample10000.txt --input /u/zhengw14/canada_us_uk.ssjoin-join.txt

/u/zhengw14/CSC2525-Cont/CSC2525-Project/ssjoin-0.1/set_sim_join_ext_stat --threshold 0.6 --cosine --algorithm ppjoin --timings --statistics --foreign-linewise --foreign-input ~/data/wikidata_white_upper_raw10.txt --input ~/data/wikidata_white_upper_raw90.txt


#finish
./script.sh ~/data/zipf-s100k-ss50-u100k-a1-dedup-raw.txt ZIPF
./script.sh ~/data/uniform-s100000-ss10-u50-dedup-raw.txt  UNIF. 
./script.sh ~/data/bms-pos-raw.txt BMS
time ./script.sh ~/data/kosarak-dedup-raw.txt KOSARAK
time ./script.sh ~/data/dblp-ss100000-upper-2q-dedup-raw.txt DBLP-2q
time ./script05.sh ~/data/AOL-user-ct-collection/aol-data-white-dedup-raw.txt AOL # 缺 0.5 cosine dice


time ./scriptjac.sh ~/data/orkut-userswithgroups-dedup-raw.txt ORKUT #只跑了 jaccard
time ./script05.sh ~/data/livejournal-userswithgroups-dedup-raw.txt LIVE # 缺 0.5 cosine dice
time ./script.sh ~/data/enron-adaptjoin-paper-dedup-dedupitems-raw.txt ENRON

time ./scriptjac08.sh ~/data/enron-adaptjoin-paper-dedup-dedupitems-lenshuf-raw.txt ENRON-shuff
time ./scriptjac08.sh ~/data/AOL-user-ct-collection/aol-data-white-dedup-lenshuf-raw.txt AOL-shuff
time ./scriptjac08.sh ~/data/kosarak-raw.txt KOSARAK-nodedup


# Running
time ./scriptjac.sh ~/canada_us_uk.ssjoin OPEN


# find ./KOSARAK-nodedup -type f -print0 | xargs -0 -n10 -P0  python3 convertOutput.py
#!/bin/bash
time ./scriptdice.sh ~/data/uniform-s100000-ss10-u50-dedup-raw90.txt ~/data/uniform-s100000-ss10-u50-dedup-raw10.txt  UNIF-CONT-XX
time ./scriptdice.sh ~/data/zipf-s100k-ss50-u100k-a1-dedup-raw90.txt ~/data/zipf-s100k-ss50-u100k-a1-dedup-raw10.txt ZIPF-CONT1-XX
time ./scriptdice.sh ~/data/bms-pos-raw90.txt ~/data/bms-pos-raw10.txt BMS-CONT-XX
time ./scriptdice.sh ~/data/kosarak-dedup-raw90.txt ~/data/kosarak-dedup-raw10.txt KOSARAK-CONT-XX
time ./scriptdice.sh ~/data/dblp-ss100000-upper-2q-dedup-raw90.txt ~/data/dblp-ss100000-upper-2q-dedup-raw10.txt DBLP-2q-CONT-XX
time ./scriptdice.sh ~/data/AOL-user-ct-collection/aol-data-white-dedup-raw90.txt ~/data/AOL-user-ct-collection/aol-data-white-dedup-raw10.txt AOL-CONT-XX


#!/bin/bash

time ./scriptdice.sh ~/data/enron-adaptjoin-paper-dedup-dedupitems-raw90.txt ~/data/enron-adaptjoin-paper-dedup-dedupitems-raw10.txt  ENRON-CONT-XX

#没跑完 0.5
time ./scriptdice.sh ~/data/livejournal-userswithgroups-dedup-raw90.txt ~/data/livejournal-userswithgroups-dedup-raw10.txt LIVE-CONT-XX
time ./scriptdice.sh ~/data/orkut-userswithgroups-dedup-raw90.txt ~/data/orkut-userswithgroups-dedup-raw10.txt ORKUT-CONT-XX


time ./cont.sh ~/canada_us_uk.ssjoin_dedup-join.txt ~/canada_us_uk.ssjoin_dedup-sample10000.txt OPEN-CONT
time ./dice.sh ~/canada_us_uk.ssjoin_dedup-join.txt ~/canada_us_uk.ssjoin_dedup-sample10000.txt OPEN-CONT-XX

time ./cont.sh ~/data/wikidata_white_upper_dedup_raw90.txt ~/data/wikidata_white_upper_dedup_raw10.txt WIKI-CONT
time ./dice.sh ~/data/wikidata_white_upper_dedup_raw90.txt ~/data/wikidata_white_upper_dedup_raw10.txt WIKI-CONT-XX


##还需要跑的
 # wikidata and KOSARAK  without removing deplicate lists
time ./diceAdapt.sh ~/data/wikidata_white_upper_raw90.txt ~/data/wikidata_white_upper_raw10.txt WIKI-CONT-XX

#!/bin/bash
# memory usage on ENRON. wikidata and opendata
time ./cont08Mem.sh ~/data/enron-adaptjoin-paper-dedup-dedupitems-raw90.txt ~/data/enron-adaptjoin-paper-dedup-dedupitems-raw10.txt ENRON-CONT-MEM
time ./cont08Mem.sh ~/data/wikidata_white_upper_dedup_raw90.txt ~/data/wikidata_white_upper_dedup_raw10.txt WIKI-CONT-MEM
time ./cont08Mem.sh ~/canada_us_uk.ssjoin_dedup-join.txt ~/canada_us_uk.ssjoin_dedup-sample10000.txt OPEN-CONT-MEM

time ./contAdapt.sh ~/data/wikidata_white_upper_raw90.txt ~/data/wikidata_white_upper_raw10.txt WIKI-CONT
time ./cont08.sh ~/data/wikidata_white_upper_raw90.txt ~/data/wikidata_white_upper_raw10.txt WIKI-CONT-nodedup
time ./cont08.sh ~/data/kosarak-raw90.txt.txt ~/data/kosarak-raw10.txt KOSARAK-CONT-nodedup
time ./jaccard08.sh ~/canada_us_uk.ssjoin_dedup-join.txt ~/canada_us_uk.ssjoin_dedup-sample10000.txt OPEN-Jaccard08

# time ./dice08.sh ~/data/wikidata_white_upper_raw90.txt ~/data/wikidata_white_upper_raw10.txt WIKI-CONT-XX-nodedup
# time ./dice08.sh ~/data/kosarak-raw90.txt.txt ~/data/kosarak-raw10.txt KOSARAK-CONT-XX-nodedup

# lenshuffle on wikidata and opendata and AOL
time ./cont08.sh ~/data/wikidata_white_upper_dedup_lenshuf_raw90.txt ~/data/wikidata_white_upper_dedup_lenshuf_raw10.txt WIKI-CONT-lenshuf
time ./cont08.sh ~/canada_us_uk.ssjoin_dedup_lenshuf-join.txt ~/canada_us_uk.ssjoin_dedup_lenshuf-sample10000.txt OPEN-CONT-lenshuf
time ./cont08.sh ~/data/AOL-user-ct-collection/aol-data-white-dedup-lenshuf-raw90.txt ~/data/AOL-user-ct-collection/aol-data-white-dedup-lenshuf-raw10.txt AOL-CONT-lenshuf

