#ifndef SSJ_ADAPTJOIN_IMPL_H
#define SSJ_ADAPTJOIN_IMPL_H

/* Copyright 2014-2015 Willi Mann
 *
 * This file is part of set_sim_join.
 *
 * set_sim_join is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with set_sim_join.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <random>
#include <algorithm>

#include "output.h"

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::addrecord(IntRecord & record) {
	inputhandler.addrecord(record);
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::addforeignrecord(IntRecord & record) {
	inputhandler.addforeignrecord(record);
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::addrawrecord(IntRecord & record) {
	inputhandler.addrawrecord(record);
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::addrawforeignrecord(IntRecord & record) {
	inputhandler.addrawforeignrecord(record);
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::preparerecords() {
	inputhandler.prepareindex();
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::prepareforeignrecords() {
	inputhandler.prepareforeign();
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::preparefinished() {
	inputhandler.cleanup();
}

template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::doindex() {
	index.largest_tokenid(inputhandler.get_largest_tokenid());
	index.index_records(indexedrecords, threshold);
}

template<typename IndexList, typename IndexedRecords>
void inline applyminsizefilter(IndexedRecords & indexedrecords, IndexList & indexlist, unsigned int minsize) {

	// Go through the list pointed to by lit - first apply length filter (min)
	typename IndexList::AlgoIndexList::iterator rit = indexlist.indexlist.begin() + indexlist.start;
	for(; rit != indexlist.indexlist.end(); ++rit) {

		const IntRecord & indexrecord = indexedrecords[rit->recordid];
		unsigned int indreclen = indexrecord.tokens.size();
		// Length filter (min). This is not in the AdaptJoin paper, but we do it anyways as it drastically
		// speeds up actual runtime (and the AdaptJoin authors were aware of the upper and lower bounds
		// on the size as they used this to estimate verification time)
		if(indreclen >= minsize) {
			break;
		}
	}
	indexlist.start = rit - indexlist.indexlist.begin();
}


template<typename Similarity, typename IndexingStrategy, typename IndexStructurePolicy>
void AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy>::dojoin(
				HandleOutput * handleoutput) {

	std::default_random_engine rngenerator;
	
	CandidateSet_ candidateSet(indexedrecords);
	
	std::vector<unsigned int> minoverlapcache;
	unsigned int lastprobesize = 0;

	std::vector<unsigned int> occnumberarray;

	// Lists to merge for the current prefix
	CollIndexLists curLists;

	// Lists to merge for the next prefix
	CollIndexLists mergeLists;


	// foreach record...
	for (unsigned recind = 0; recind < proberecordssize(); ++recind) {
		typename Index::ProbeRecord & record = getproberecord(indexedrecords, foreignrecords, recind);
		unsigned int reclen = record.tokens.size();

		unsigned int maxprefix = Similarity::maxprefix(reclen, threshold);
		unsigned int minsize = Similarity::minsize(reclen, threshold);

		// Check whether cache is to renew
		if(lastprobesize != reclen) {
			lastprobesize = reclen;
			unsigned int maxel = Index::SELF_JOIN ? reclen : Similarity::maxsize(reclen, threshold);
			minoverlapcache.resize(maxel + 1);
			for(unsigned int i = minsize; i <= maxel; ++i) {
				minoverlapcache[i] = Similarity::minoverlap(reclen, i, threshold);
			}
		}

		typename IndexingStrategy::template maxsizechecker<self_type> maxsizechecker(reclen, threshold);

		unsigned int verifyCostPerCand;
#if 0
		// Assuming self-join, the average "verification-cost" of a pair
		if(Index::SELF_JOIN) {
			verifyCostPerCand = ((minsize + reclen) / 2 + reclen) / 256 + 1;
		} else {
#endif
			//Stick to cost function in paper
			unsigned int maxsize = Similarity::maxsize(reclen, threshold);
			verifyCostPerCand = ((minsize + maxsize) / 2 + reclen);
#if 0
		}
#endif
		
		// Array to store how often (index) how many indexrecords (value) were seen
		occnumberarray.clear();
		occnumberarray.resize(maxprefix + IndexStructure::MAX_PREFIX_ELL + 1, 0);

		//the sum of the lengths of all the lists that have been merged
		unsigned int curListsLenSum = 0;

		// Current prefix extension. 0 means, the 1-prefix is used (\ell - 1)
		unsigned int curPreExt = 0;

		// count candidates with more than \ell matches
		unsigned int moreThanEll = 0;

		// count candidates with less than \ell matches including unavoidable candidates
		unsigned int lessThanEll = 0;

		// count candidates with exactly \ell matches
		unsigned int exactlyEll = 0;

		// clear curLists
		curLists.clear();
		
		//collect candidates in 1-prefix
		for(unsigned int recpos = 0; recpos < maxprefix; ++recpos) {
			unsigned int token = record.tokens[recpos];
			
			IndexList * il = index.getindexlist(token, 0);
			statistics.lookups.inc();
			// if token is in index: append list to lists to merge
			if( il != NULL) {
				applyminsizefilter<IndexList, IndexedRecords>(indexedrecords, *il, minsize);
				typename IndexList::AlgoIndexList::iterator rit = il->indexlist.begin() + il->start;
				for(; rit != il->indexlist.end(); ++rit) {

					const IntRecord & indexrecord = indexedrecords[rit->recordid];
					unsigned int indreclen = indexrecord.tokens.size();

					if(!IndexingStrategy::recindchecker::istocheck(recind, rit->recordid)) {
						break;
					}

					//Length filter (max)
					if(maxsizechecker.isabove(indreclen)) {
						break;
					}

					statistics.indexEntriesSeen.inc();

					// Check whether record was already merged
					CandidateData & candidateData = candidateSet.getCandidateData(rit->recordid);

					if(candidateData.count == 0) {
						// candidate not yet seen
						candidateSet.addRecord(rit->recordid);
					}

					candidateData.count += 1;

					occnumberarray[candidateData.count] += 1;
				}

				curListsLenSum += il->indexlist.size();
			}
		}

		lessThanEll = 0;
		exactlyEll = occnumberarray[1] - occnumberarray[2];
		moreThanEll = candidateSet.size() - exactlyEll;

		while (true) {

			// Make sure to not run into case where prefix extension gets larger
			// than the minimum overlap possible with the current probing set (equals minsize)
			if(curPreExt + 1 == IndexStructure::MAX_PREFIX_ELL || curPreExt + 1 >= minsize) {
				break;
			}

			//clear mergelists
			mergeLists.clear();

			// sum of merge list lengths
			unsigned int mergeListsLen = 0;


			// Collect lists to merge for following prefix extension
			for(unsigned int rp2 = curPreExt; rp2 < maxprefix + curPreExt; ++rp2) {
				IndexList * il = index.getindexlist(record.tokens[rp2], curPreExt + 1);
				statistics.lookups.inc();
				if(il == NULL) {
					continue;
				}
				mergeLists.push_back(IndexListCollEntry(il, curPreExt + 1, rp2));
				applyminsizefilter<IndexList, IndexedRecords>(indexedrecords, *il, minsize);
				mergeListsLen += il->indexlist.size() - il->start;
			}


			// Now collect lists caused by new token
			unsigned int token = record.tokens[maxprefix + curPreExt];

			for(unsigned i = 0; i < curPreExt + 2; ++i) {
				IndexList * il = index.getindexlist(token, i);
				statistics.lookups.inc();
				if(il != NULL) {
					mergeLists.push_back(IndexListCollEntry(il, i, maxprefix + curPreExt));
					applyminsizefilter<IndexList, IndexedRecords>(indexedrecords, *il, minsize);
					mergeListsLen += il->indexlist.size() - il->start;
				}
			}

			//Now compute max(K, mergeListsLen) random indices to choose random 
			//records from the mergeLists
			std::vector<unsigned int> randomindices;


			if(mergeListsLen <= K) {
				randomindices.resize(mergeListsLen);
				std::iota(randomindices.begin(), randomindices.end(), 0);
			} else {
				// Partition into K ranges, select 1 element from each range
				// add to randomindices - so randomindices is sorted 
				randomindices.reserve(K);
				for(unsigned int i = 0; i < K; ++i) {
					unsigned int beginrange = mergeListsLen * i / K;
					unsigned int endrange = mergeListsLen * (i + 1) / K;
					std::uniform_int_distribution<unsigned int> distribution(beginrange, endrange -1);
					unsigned int dice_roll = distribution(rngenerator);
					randomindices.push_back(dice_roll);
				}
			}

			//collect number of elements where $s_i \in \mathcal{C}_\ell^=(r)$
			unsigned int elemLenEqCurPre = 0;
			
			std::vector<unsigned int>::iterator curElemIt = randomindices.begin();
			typename CollIndexLists::iterator mergeListIt = mergeLists.begin();
			unsigned int cur0index = 0;

			while(curElemIt != randomindices.end()) {
				assert(mergeListIt != mergeLists.end());
				unsigned int curMergeListSize = mergeListIt->indexlist->indexlist.size() - mergeListIt->indexlist->start;
				if( *curElemIt >= cur0index + curMergeListSize) {
					//Go to next list
					cur0index += curMergeListSize;
					++mergeListIt;
					continue;
				}
				// Check whether the sample occurs exactly \ell times in current lists
				CandidateData & candidateData =
					candidateSet.getCandidateData(mergeListIt->indexlist->indexlist[*curElemIt + mergeListIt->indexlist->start - cur0index].recordid);
				
				if(candidateData.count == curPreExt + 1) {
					elemLenEqCurPre += 1;
				}
				++curElemIt;
			}

			// Number of candidates that require verification
			unsigned int candSizeCurPref = moreThanEll + exactlyEll;

			// Verification cost
			unsigned int vrfCostCurPref = candSizeCurPref * verifyCostPerCand;

			// Cost of filtering for the current prefix
			unsigned int fltCostCurPref = curListsLenSum;

			// full cost of current prefix
			unsigned int costCurPref = vrfCostCurPref + fltCostCurPref;


			// estimated Candidate size for next prefix
			unsigned int estCandSizeNextPref = moreThanEll + 
				mergeListsLen * elemLenEqCurPre / std::max((unsigned int)1,std::min(mergeListsLen, (unsigned int)K));

			// verification cost for next prefix
			unsigned int vrfCostNextPref = estCandSizeNextPref * verifyCostPerCand;

			// filter cost for next prefix
			unsigned int fltCostNextPref = fltCostCurPref + mergeListsLen;

			//full (estimated) cost of next prefix
			unsigned int costNextPref = vrfCostNextPref + fltCostNextPref;

			// number of lists that have to be merged for 2nd next prefix
			unsigned int lstMrg2ndNextPref = maxprefix + 2 * (curPreExt + 3) - 2;

			// estimation Cost for the 2nd next prefix
			// Note: The estimation does not really match what we implement
			// since we don't need binary search to find the lists. 
			unsigned int estCost2ndNextPref = 2 * lstMrg2ndNextPref + (unsigned int)(K * log(lstMrg2ndNextPref)) + K;
			
			if( costCurPref > costNextPref + estCost2ndNextPref && curPreExt < IndexStructure::MAX_PREFIX_ELL - 1) {
				moreThanEll -= occnumberarray[curPreExt + 2] - occnumberarray[curPreExt + 3];
				exactlyEll += occnumberarray[curPreExt + 2] - occnumberarray[curPreExt + 3];
				exactlyEll -= occnumberarray[curPreExt + 1] - occnumberarray[curPreExt + 2];
				lessThanEll += occnumberarray[curPreExt + 1] - occnumberarray[curPreExt + 2];

				assert(moreThanEll + exactlyEll + lessThanEll == candidateSet.size());

				curPreExt += 1;
				mergeLists.swap(curLists);
			} else {
				break;
			}

			// Store value of moreThanEl
			unsigned int exactlyEllBefore = exactlyEll;

			// Store value of lessThanEl
			unsigned int lessThanEllBefore = lessThanEll;

			// Go through lists to merge and merge
			typename CollIndexLists::iterator lit = curLists.begin();
			for( ; lit != curLists.end(); ++lit) {


				typename IndexList::AlgoIndexList::iterator rit = lit->indexlist->indexlist.begin() + lit->indexlist->start;

				assert(lit->indexlist->indexlist.begin() + lit->indexlist->start == rit);

				for(; rit != lit->indexlist->indexlist.end(); ++rit) {

					const IntRecord & indexrecord = indexedrecords[rit->recordid];
					unsigned int indreclen = indexrecord.tokens.size();

					if(!IndexingStrategy::recindchecker::istocheck(recind, rit->recordid)) {
						break;
					}

					//Length filter (max)
					if(maxsizechecker.isabove(indreclen)) {
						break;
					}

					statistics.indexEntriesSeen.inc();

					// Check whether record was already merged
					CandidateData & candidateData = candidateSet.getCandidateData(rit->recordid);

					if(candidateData.count < curPreExt) {
						//candidate was not yet seen (or not often enough), so we discard it
						continue;
					}


#ifndef NDEBUG
					// if number of occurences is now 1 larger than current prefix extension
					// change the counters it concerns
					if(candidateData.count == curPreExt + 1) {
						moreThanEll += 1;
						exactlyEll -= 1;
					}
					//if number of occurrences is now exactly the current prefix extension
					else if(candidateData.count == curPreExt) {
						exactlyEll += 1;
						if(candidateData.count != 0) {
							lessThanEll -= 1;
						}
					}
#endif

					candidateData.count += 1;

					occnumberarray[candidateData.count] += 1;

				}

				curListsLenSum += lit->indexlist->indexlist.size();
			}

			// Assert for !NDEBUG case that the assumptions to compute lessThanEll
			assert(lessThanEll == occnumberarray[1] - occnumberarray[curPreExt + 1]);
			lessThanEll = (occnumberarray[1] - occnumberarray[curPreExt + 1]);


			// Assert for !NDEBUG case that the assumptions to compute lessThanEll
			assert(exactlyEll == occnumberarray[curPreExt + 1] - occnumberarray[curPreExt + 2]);
			exactlyEll = occnumberarray[curPreExt + 1] - occnumberarray[curPreExt + 2];

			// Assert for !NDEBUG case that the assumptions to compute lessThanEll
			assert(moreThanEll == candidateSet.size() - lessThanEll - exactlyEll);
			moreThanEll = candidateSet.size() - lessThanEll - exactlyEll;


			if(curPreExt + 1 == IndexStructure::MAX_PREFIX_ELL || curPreExt + 1 >= minsize) {
				break;
			}

		}

		statistics.adaptjoinlastext[std::min<unsigned int>(curPreExt, _Statistics::MAX_ADAPTJOIN_EXT - 1)].inc();

		statistics.candidatesP1.add(candidateSet.size());
		
		//Now, verify candidates
		typename CandidateSet_::iterator candit = candidateSet.begin();
		for( ; candit != candidateSet.end(); ++candit) {
			// record from index 
			CandidateData & candidateData = candidateSet.getCandidateData(*candit);
#if defined(CAND_ONLY)
			if(candidateData.count > curPreExt ) {
				statistics.candidatesVery.inc();
			}
#else
			if(candidateData.count <= curPreExt) {
				candidateData.reset();
				continue;
			}

			const IndexedRecord & indexrecord = indexedrecords[*candit];

			unsigned int indreclen = indexrecord.tokens.size();
			
			unsigned int minoverlap = minoverlapcache[indreclen];
#endif
#if defined(CAND_ONLY)
			//Nothing to do anymore
#else

			// Special case: Already enough overlap found
			if(candidateData.count >= minoverlap) {
				handleoutput->addPair(record, indexrecord);
				candidateData.reset();
				statistics.candidatesVery.inc();
				continue;
			}

			unsigned int recpreflen = maxprefix + curPreExt;
			unsigned int lasttokenrec = record.tokens[recpreflen - 1];

			unsigned int indrecpreflen = std::min(indreclen, indexrecord.indexprefixsize + curPreExt);
			unsigned int lasttokenindrec = indexrecord.tokens[indrecpreflen - 1];
			
			unsigned int recpos, indrecpos;

			if(lasttokenrec < lasttokenindrec) {
				if( candidateData.count + reclen - recpreflen < minoverlap) {
					candidateData.reset();
					continue;
				} else {
					recpos = recpreflen;
					indrecpos = candidateData.count;
				}
			} else {
				if( candidateData.count + indreclen - indrecpreflen < minoverlap) {
					candidateData.reset();
					continue;
				} else {
					recpos = candidateData.count;
					indrecpos = indrecpreflen;
				}
			}
			statistics.candidatesVery.inc();

			if(verifypair(record.tokens, indexrecord.tokens, minoverlap, recpos, indrecpos, candidateData.count)) {
				handleoutput->addPair(record, indexrecord);
			}
#endif
			candidateData.reset();

		}
		candidateSet.clear();
		
		//Put record to index
		index.index_record(record, recind, reclen, threshold);
	}

}

#endif
