#ifndef SSJ_ADAPTJOIN_H
#define SSJ_ADAPTJOIN_H

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

#include <algorithm>
#include "classes.h"
#include "verify.h"
#include "data.h"
#include "frequencysorting.h"
#include "adaptjoin_indexes.h"
#include "candidateset.h"


template<typename AdaptJoinSimilarity=Jaccard, typename IndexingStrategy = AdaptJoinIndexFirstPolicy<true>, typename IndexStructurePolicy = AdaptJoinIndexStructurePolicy >
class AdaptJoin: public Algorithm {
	public:
		typedef typename AdaptJoinSimilarity::threshold_type threshold_type;
		const threshold_type threshold;
		// No further specialization needed
		//
		enum {
			K = 3
		};

		struct CandidateData {
			unsigned int count;
			unsigned int recpos;
			CandidateData() : count(0), recpos(0) {}
			inline void reset() {
				count = 0;
			}
		};

		typedef IntRecord BaseRecord;
		typedef IntRecord ForeignRecord;

		class IndexedRecord : public ForeignRecord {
			public:
				unsigned int indexprefixsize;
				CandidateData candidateData;
		};

		typedef std::vector<IndexedRecord> IndexedRecords;
		typedef std::vector<ForeignRecord> ForeignRecords;

		IndexedRecords indexedrecords;
		ForeignRecords foreignrecords;

		algo_handle_records_freq_sort<IndexedRecords, ForeignRecords> inputhandler;

		typedef AdaptJoinSimilarity Similarity;
		typedef AdaptJoin<Similarity, IndexingStrategy, IndexStructurePolicy> self_type;
		typedef typename IndexStructurePolicy::template IndexStructure<self_type> IndexStructure;
		typedef typename IndexStructure::IndexList IndexList;

		struct IndexListCollEntry {
			IndexList * indexlist;
			const unsigned int prefExt;
			const unsigned int position;
			IndexListCollEntry(IndexList * indexlist, const unsigned int prefExt, unsigned int position) :
				indexlist(indexlist), prefExt(prefExt), position(position) {}
		};

		typedef std::vector<IndexListCollEntry> CollIndexLists;

		typedef CandidateSet<CandidateData, IndexedRecords> CandidateSet_;

		
		typedef typename IndexingStrategy::template Index<self_type> Index;
		Index index;

	public:
		AdaptJoin(threshold_type threshold) : threshold(threshold), inputhandler(indexedrecords, foreignrecords) {}
		//addrecord and addforeignrecord must use swap to get the integer vector from record,
		//such that record refers to an empty record after the respective calls
		void addrecord(IntRecord & record);
		void addforeignrecord(IntRecord & record);

		//addrawrecord and addrawforeignrecord must use swap to get the integer vector from record,
		//such that record refers to an empty record after the respective calls
		void addrawrecord(IntRecord & record);
		void addrawforeignrecord(IntRecord & record);

		//multi-step process to measure the individual steps from outside
		void preparerecords();
		void prepareforeignrecords();
		void preparefinished();
		void doindex();
		void dojoin(
				HandleOutput * handleoutput);

	private:
		inline size_t proberecordssize() {
			if(Index::SELF_JOIN) {
				return indexedrecords.size();
			} else {
				return foreignrecords.size();
			}
		}
		GetProbeRecord<Index::SELF_JOIN,  ForeignRecords, IndexedRecords,
			IndexedRecord, typename Index::ProbeRecord> getproberecord;
};

#include "adaptjoin_impl.h"

#endif
