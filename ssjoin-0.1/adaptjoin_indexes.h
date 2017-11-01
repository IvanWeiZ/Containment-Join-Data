#ifndef SSJ_ADAPTJOIN_INDEXES_H
#define SSJ_ADAPTJOIN_INDEXES_H

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

#include <vector>
#include "mpltricks.h"
#include "indexes.h"
#include "inv_index.h"


// For adaptjoin, we put the index structure and the indexing strategy into the same file
// as there is only one index structure


template <typename Algorithm>
class AdaptJoinCommonMaxExtStructures {
	public:
		/* For this algorithm, we use
		 * - the IndexListHeader, which exists once per token,
		 * - the IndexList, which might exist multiple times per token but only one
		 *   for each prefix extension (delta lists)
		 * - the IndexListEntries, which might occur multiple times within an index list
		 */
		struct IndexListEntry : GlobalGenericIndexListEntry {
			IndexListEntry(unsigned int recordid) : GlobalGenericIndexListEntry(recordid) {}
		};

		struct IndexList : GenericIndexListHeader<IndexListEntry> {
			unsigned int start;
			IndexList() : start(0) {}
		};


		struct IndexListHeader {
			IndexList indexlists[Algorithm::IndexStructure::MAX_PREFIX_ELL];

			void recordadd(unsigned int prefixnmb, unsigned int recordid) {
				assert(prefixnmb < Algorithm::IndexStructure::MAX_PREFIX_ELL);
				indexlists[prefixnmb].indexlist.push_back(IndexListEntry(recordid));
			}

			IndexList * getlist(unsigned int prefixnmb) {
				return &indexlists[prefixnmb];
			}

		};

};

template <typename InvIndexMap>
class AdaptJoinIndexMaxExtStructurePolicy {

	public:
		template <typename Algorithm>
		struct IndexStructure {

			enum {
				MAX_PREFIX_ELL = 8
			};

			typedef typename AdaptJoinCommonMaxExtStructures<Algorithm>::IndexListEntry IndexListEntry;
			typedef typename AdaptJoinCommonMaxExtStructures<Algorithm>::IndexList IndexList;
			typedef typename AdaptJoinCommonMaxExtStructures<Algorithm>::IndexListHeader IndexListHeader;

			typedef typename InvIndexVector::template IndexMap<IndexListHeader> Index;
			typedef typename Algorithm::IndexedRecord IndexedRecord;
			Index index;


			inline void addtoken(unsigned int token, unsigned int recind, unsigned int recpos, unsigned int prefixext) {
				typename Index::value_type & ilhead = index.get_list_create(token);
				ilhead.recordadd(prefixext, recind);
			}

			inline IndexList * getindexlist(unsigned int token, unsigned int prefixext) {
				typename Index::value_type * ilhead = index.get_list(token);
				if( ilhead != NULL ) {
					return ilhead->getlist(prefixext);
				} else {
					return NULL;
				}
			}

			void largest_tokenid(unsigned int tokenid) {
				index.resize(tokenid + 1);
			}
		};
};


template <typename Algorithm>
class AdaptJoinCommonStructures {
	public:
		/* For this algorithm, we use
		 * - the IndexListHeader, which exists once per token,
		 * - the IndexList, which might exist multiple times per token but only one
		 *   for each prefix extension (delta lists)
		 * - the IndexListEntries, which might occur multiple times within an index list
		 */
		struct IndexListEntry : GlobalGenericIndexListEntry {
			IndexListEntry(unsigned int recordid) : GlobalGenericIndexListEntry(recordid) {}
		};

		struct IndexList : GenericIndexListHeader<IndexListEntry> {
			unsigned int start;
			IndexList() : start(0) {}
		};

		struct IndexListPtr {
			IndexList * il;
			IndexListPtr() : il(NULL) {}
		};


		struct IndexBase {
			typedef InvIndexHashMap::template IndexMap<IndexListPtr> IndexEll;
			IndexEll indexells[Algorithm::IndexStructure::MAX_PREFIX_ELL];
			IndexList emptyindexlist;

			inline void recordadd(unsigned int token, unsigned int prefixnmb, unsigned int recordid) {
				assert(prefixnmb < Algorithm::IndexStructure::MAX_PREFIX_ELL);
				IndexListPtr & ilp = indexells[prefixnmb].get_list_create(token);
				if(ilp.il == NULL) {
					ilp.il = new IndexList();
				}
				ilp.il->indexlist.push_back(IndexListEntry(recordid));
			}

			inline IndexList * getlist(unsigned int token, unsigned int prefixnmb) {
				assert(prefixnmb < Algorithm::IndexStructure::IndexStructure::MAX_PREFIX_ELL);
				IndexEll & ie = indexells[prefixnmb];
				IndexListPtr * ilp = ie.get_list(token);
				if(ilp == NULL) {
					return NULL;
				} else {
					return ilp->il;
				}
			}

			~IndexBase() {
				for(unsigned int psch = 0; psch < Algorithm::IndexStructure::MAX_PREFIX_ELL; ++psch) {
					typename IndexEll::iterator it = indexells[psch].begin();
					for( ; it != indexells[psch].end(); ++it) {
						delete it->second.il;
					}
				}
			}
		};

};

class AdaptJoinIndexStructurePolicy {

	public:

		template <typename Algorithm>
		struct IndexStructure {
			enum {
				MAX_PREFIX_ELL = 1000
			};


			typedef typename AdaptJoinCommonStructures<Algorithm>::IndexListEntry IndexListEntry;
			typedef typename AdaptJoinCommonStructures<Algorithm>::IndexList IndexList;
			typedef typename AdaptJoinCommonStructures<Algorithm>::IndexBase IndexBase;

			//typedef typename InvIndexVector::template IndexMap<IndexListHeader> Index;
			typedef typename Algorithm::IndexedRecord IndexedRecord;
			IndexBase indexbase;


			inline void addtoken(unsigned int token, unsigned int recind, unsigned int recpos, unsigned int prefixext) {
				indexbase.recordadd(token, prefixext, recind);
			}

			inline IndexList * getindexlist(unsigned int token, unsigned int prefixext) {
				return indexbase.getlist(token, prefixext);
			}

			inline void largest_tokenid(unsigned int tokenid) {
				// nothing to do
			}
		};
};


class AdaptJoinIndexOnTheFlyPolicy {
	public:

		template <class Algorithm>
		struct Index {
			enum {
				SELF_JOIN = true,
				INDEXFIRST = false
			};
			typedef typename Algorithm::IndexStructure IndexStructure;
			typedef typename Algorithm::IndexList IndexList;
			typedef typename Algorithm::Similarity Similarity;
			typedef typename Algorithm::IndexedRecords IndexedRecords;
			typedef typename Algorithm::IndexedRecord IndexedRecord;
			typedef typename Algorithm::BaseRecord BaseRecord;

			typedef typename Algorithm::IndexedRecord ProbeRecord;

			IndexStructure index;

			typedef typename Similarity::threshold_type threshold_type;

			void largest_tokenid(unsigned int size_universe) {
				index.largest_tokenid(size_universe);
			}
			void index_records(const IndexedRecords & records, threshold_type threshold) {}

			inline void index_record(IndexedRecord & record, unsigned int recind, unsigned int reclen, threshold_type threshold) {
				record.indexprefixsize = Similarity::midprefix(reclen, threshold);
				unsigned int incindex = 0;
				unsigned int pos = 0;

				while(incindex < Algorithm::IndexStructure::MAX_PREFIX_ELL && pos < reclen) {
					unsigned int token = record.tokens[pos];

					index.addtoken(token, recind, pos, incindex);

					pos += 1;
					if(pos >= record.indexprefixsize) {
						incindex += 1;
					}
				}

			}

			inline IndexList * getindexlist(unsigned int token, unsigned int prefixExt) {
				return index.getindexlist(token, prefixExt);
			}
		};

		template <class Algorithm>
		struct maxsizechecker {
			unsigned int maxlen;
			unsigned int curlen;

			typedef typename Algorithm::Similarity::threshold_type threshold_type;
			threshold_type threshold;

			inline maxsizechecker(unsigned int curlen, threshold_type threshold) : curlen(curlen), threshold(threshold) {
				maxlen = Algorithm::Similarity::maxsize(curlen, threshold);
			}

			inline bool isabove(unsigned int len) {
			//	if(Algorithm::LengthFilterPolicy::POS) {
			//		return len > maxlen;
			//	} else {
					return false;
			//	}
			}
			
/*			inline void updateprobepos(unsigned int pos) {
				if(Algorithm::LengthFilterPolicy::POS) {
					maxlen = Algorithm::Similarity::maxsize(curlen, pos, threshold);
				}
			}*/

		};
		
		struct recindchecker {
			inline static bool istocheck(unsigned int reclen, unsigned int indreclen) {
				return true;
			}
		};


};

template <bool foreign>
class AdaptJoinIndexFirstPolicy {
	public:

		template <class Algorithm>
		struct Index {
			enum {
				SELF_JOIN = !foreign,
				INDEXFIRST = true
			};
			typedef typename Algorithm::IndexStructure IndexStructure;
			typedef typename Algorithm::Similarity Similarity;
			typedef typename Algorithm::IndexedRecords IndexedRecords;
			typedef typename Algorithm::IndexedRecord IndexedRecord;
			typedef typename Algorithm::BaseRecord BaseRecord;
			typedef typename Algorithm::IndexList IndexList;

			IndexStructure index;

			typedef typename Similarity::threshold_type threshold_type;

			// Template metaprogramming trickery to select right ProbeRecord type
			typedef typename IF<foreign, typename Algorithm::ForeignRecord, typename Algorithm::IndexedRecord>::RET ProbeRecord;

			void largest_tokenid(unsigned int size_universe) {
				index.largest_tokenid(size_universe);
			}

			inline unsigned int indexPrefixSize(unsigned int reclen, threshold_type threshold) const {
			 return foreign ? Similarity::maxprefix(reclen, threshold) : Similarity::midprefix(reclen, threshold);
			}

			void index_records(IndexedRecords & records, threshold_type threshold) {
				unsigned int recind = 0;
				for(; recind < records.size(); ++recind) {
					IndexedRecord & record = records[recind];
					unsigned int reclen = record.tokens.size();
					record.indexprefixsize = indexPrefixSize(reclen, threshold);
					unsigned int recpos = 0;
					for(; recpos < record.indexprefixsize; ++recpos) {
						unsigned int token = record.tokens[recpos];
						index.addtoken(token, recind, recpos, 0);
					}
					
					for(unsigned int i = 1; recpos < record.tokens.size() && i < Algorithm::IndexStructure::MAX_PREFIX_ELL; ++recpos, ++i) {
						unsigned int token = record.tokens[recpos];
						index.addtoken(token, recind, recpos, i);
					}
					
				}
			}

			inline void index_record(const BaseRecord & record, unsigned int recind, 
					unsigned int reclen, threshold_type threshold) {}

			inline IndexList * getindexlist(unsigned int token, unsigned int prefixExt) {
				return index.getindexlist(token, prefixExt);
			}
		};
		
		template <class Algorithm>
		struct maxsizechecker {
			unsigned int curlen;

			typedef typename Algorithm::Similarity::threshold_type threshold_type;
			threshold_type threshold;

			unsigned int maxlen;
			inline maxsizechecker(unsigned int curlen, threshold_type threshold) : curlen(curlen), threshold(threshold) {
				maxlen = Algorithm::Similarity::maxsize(curlen, threshold);
			}

/*			inline void updateprobepos(unsigned int pos) {
				if(Algorithm::LengthFilterPolicy::POS) {
					maxlen = Algorithm::Similarity::maxsize(curlen, pos, threshold);
				}
			}*/

			inline bool isabove(unsigned int len) {
				return len > maxlen;
			}
		};

		struct recindchecker {
			inline static bool istocheck(unsigned int recid, unsigned int indrecid) {
				return foreign ? true : recid > indrecid;
			}
		};


};


#endif
