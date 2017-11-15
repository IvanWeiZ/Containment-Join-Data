#ifndef SSJ_LSH_H
#define SSJ_LSH_H

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

#include <sparsehash/dense_hash_set>

#include "config.h"

#ifdef USECITYHASH
	#include <city.h>
	#define LSHAVAILABLE
#endif

#include "classes.h"
#include "output.h"
#include "verify.h"
#include "statistics.h"

class LSHCommon {
	private:
		void generate_hash_params(unsigned int dimensions);
		enum { k = 3 };
		unsigned int l;

	public:
		struct HashParam {
			unsigned int a;
			unsigned int b;
		};

		typedef std::vector< std::vector<HashParam> > HashParams;

		//hash_params [l] [k]
		HashParams hash_params;

		unsigned int module;

		LSHCommon() : l(0) {}
		inline void set_dimensions(unsigned int dims) {
			assert(l != 0);
			generate_hash_params(dims);
		}

		inline void set_l(unsigned int l) {
			this->l = l;
			hash_params.resize(l);
			for(unsigned int i = 0; i < l; ++i) {
				hash_params[i].resize(k);
			}
		}

		inline unsigned int get_k() const {
			return k;
		}

		inline unsigned int get_l() const {
			return l;
		}

		
};


template<class Similarity>
class LSH : public Algorithm {

	public:
		const double threshold;

		typedef IntRecord IndexedRecord;
		typedef IntRecord ForeignRecord;

		typedef std::vector<IndexedRecord> IndexedRecords;
		typedef std::vector<ForeignRecord> ForeignRecords;

		IndexedRecords indexedrecords;
		ForeignRecords foreignrecords;

		LSHCommon lshcommon;
		unsigned int dimensions;

		typedef std::vector<unsigned int> Signature;

		struct Hasher {
			uint64_t operator()(const Signature * vec) const {
#ifdef USECITYHASH
				return CityHash64((const char*) &(*vec)[0], sizeof(Signature::value_type) * vec->size());
#else
				return 0;
#endif
			}
		};
		struct EqSignature {
			bool operator()(const Signature * s1, const Signature * s2) const {
				if(s1 == NULL) {
					if(s2 == NULL) {
						return true;
					} else {
						return false;
					}
				} else if(s2 == NULL) {
					return false;
				} else {
					return *s1 == *s2;
				}
			}
		};


		typedef std::vector<unsigned int> IndexRecordList;
		typedef google::dense_hash_map<Signature*, IndexRecordList, Hasher, EqSignature > SigHashMap;
		typedef std::vector<SigHashMap> Index;
		Index index;

		typedef google::dense_hash_set< unsigned int > Candidates;

		LSH(double threshold) : threshold(threshold), dimensions(0) {
			//TODO: This is for jaccard - cosine and dice need different function
			double basis = 1 - pow(threshold, lshcommon.get_k());
			lshcommon.set_l((unsigned int)(ceil(log(1 - 0.95)/log(basis))));

			statistics.lshL.add(lshcommon.get_l());

			index.resize(lshcommon.get_l());
			for(unsigned int i = 0; i < lshcommon.get_l(); ++i) {
				index[i].set_empty_key(NULL);
			}
		}

		typedef std::vector<Signature*> SignaturesToCleanup;
		SignaturesToCleanup signaturestocleanup;

		//addrecord and addforeignrecord must use swap to get the integer vector from record,
		//such that record refers to an empty record after the respective calls
		virtual void addrecord(IntRecord & record);
		virtual void addforeignrecord(IntRecord & record);

	private:
		//addrecord and addforeignrecord must use swap to get the integer vector from record,
		//such that record refers to an empty record after the respective calls
		virtual void addrecord(IntRecord & record, bool dosort);
		virtual void addforeignrecord(IntRecord & record, bool dosort);

	public:
		//addrawrecord and addrawforeignrecord must use swap to get the integer vector from record,
		//such that record refers to an empty record after the respective calls
		virtual void addrawrecord(IntRecord & record);
		virtual void addrawforeignrecord(IntRecord & record);

		//multi-step process to measure the individual steps from outside
		virtual void preparerecords();
		virtual void prepareforeignrecords();
		//preparation of input finished - method can be used to clean up data structures from preparation phase
		virtual void preparefinished() {};
		virtual void doindex();
		virtual void dojoin(
				HandleOutput * handleoutput);
		virtual ~LSH();

		typedef std::vector<Signature*> Signatures;

		inline void compute_signatures_record(const IntRecord & record, Signatures & sigs) {
			sigs.clear();
			sigs.resize(lshcommon.get_l());
			for(unsigned i = 0; i < lshcommon.get_l(); ++i) {
				sigs[i] = new Signature();
				signaturestocleanup.push_back(sigs[i]);
				sigs[i]->resize(lshcommon.get_k());
				for(unsigned j = 0; j < lshcommon.get_k(); ++j) {
					LSHCommon::HashParam & hp = lshcommon.hash_params[i][j];
					IntRecord::Tokens::const_iterator tit = record.tokens.begin();
					unsigned int current_min_token = INT_MAX;
					unsigned int current_min_hash = INT_MAX;
					for(; tit != record.tokens.end(); ++tit) {
						unsigned int tokhash = (*tit * hp.a + hp.b) % lshcommon.module;
						if(tokhash < current_min_hash) {
							current_min_token = *tit;
							current_min_hash = tokhash;
						}
					}
					sigs[i]->push_back(current_min_token);
				}
			}
		}
};

template<class Records, class RecordType>
inline RecordType & abstract_addrecord(IntRecord & record, Records & records, bool dosort=true) {
	// Create new record in indexedrecords
	records.push_back(RecordType());
	RecordType & newrecord = records[records.size() - 1];
	newrecord.recordid = records.size() - 1;
	newrecord.tokens.swap(record.tokens);

	//Sort tokens - neeeded for verification routine
	if(dosort) {
		std::sort(newrecord.tokens.begin(), newrecord.tokens.end());
	}
	return newrecord;
}

template<class Similarity>
void LSH<Similarity>::addrecord(IntRecord & record, bool dosort) {

	IndexedRecord & newrecord = abstract_addrecord<IndexedRecords, IndexedRecord>(record, indexedrecords, dosort);

	//We assume that the token ids are dense -
	// get number of dimensions
	IntRecord::Tokens::iterator tit = newrecord.tokens.begin();
	for(; tit != newrecord.tokens.end(); ++tit) {
		if(dimensions < *tit) {
			dimensions = *tit;
		}
	}
}

template<class Similarity>
void LSH<Similarity>::addrecord(IntRecord & record) {
	addrecord(record, true);
}

template<class Similarity>
void LSH<Similarity>::addforeignrecord(IntRecord & record, bool dosort) {
	abstract_addrecord<ForeignRecords, ForeignRecord>(record, foreignrecords, dosort);
}

template<class Similarity>
void LSH<Similarity>::addforeignrecord(IntRecord & record) {
	addforeignrecord(record, true);
}
template<class Similarity>
void LSH<Similarity>::addrawrecord(IntRecord & record) {
	addrecord(record, false);
}

template<class Similarity>
void LSH<Similarity>::addrawforeignrecord(IntRecord & record) {
	addforeignrecord(record, false);
}

template<class Similarity>
void LSH<Similarity>::preparerecords() {
}

template<class Similarity>
void LSH<Similarity>::prepareforeignrecords() {
}

template<class Similarity>
void LSH<Similarity>::doindex() {
	lshcommon.set_dimensions(dimensions);
}


template<class Similarity>
void LSH<Similarity>::dojoin(HandleOutput * handleoutput) {
	Signatures sigs;
	Candidates cands;
	cands.set_empty_key(INT_MAX);
	for(unsigned int recind = 0; recind < indexedrecords.size(); ++recind) {

		IntRecord & record = indexedrecords[recind];
		unsigned int reclen = record.tokens.size();
		unsigned int minsize = Similarity::minsize(reclen, threshold);
		unsigned int maxsize = Similarity::maxsize(reclen, threshold);
		cands.clear();

		//Get signatures of current record
		compute_signatures_record(record, sigs);

		// collect candidates based on matching signatures
		for(unsigned int j = 0; j < lshcommon.get_l(); ++j) {
			typename SigHashMap::iterator sit = index[j].find(sigs[j]);
			if(sit != index[j].end()) {
				IndexRecordList::iterator irit = sit->second.begin();
				for( ; irit != sit->second.end(); ++irit) {
					statistics.indexEntriesSeen.inc();
					unsigned int indrecsize = indexedrecords[*irit].tokens.size();
					// Do very trivial length filtering first, because minoverlap expects to only
					// get pairs within [minsize:maxsize]
					if(indrecsize < minsize || indrecsize > maxsize) {
						continue;
					}
					cands.insert(*irit);
				}
			}
		}

		statistics.candidatesP1.add(cands.size());

		//verify candidates
		Candidates::iterator candit = cands.begin();
		for(; candit != cands.end() ; ++candit) {
			IntRecord & indexedrecord = indexedrecords[*candit];
			unsigned int minoverlap = Similarity::minoverlap(reclen, indexedrecord.tokens.size(), threshold);
			if(verifypair(record.tokens, indexedrecord.tokens, minoverlap)) {
				handleoutput->addPair(record, indexedrecord);
			}
		}

		//put to index
		for(unsigned int j = 0; j < lshcommon.get_l(); ++j) {
			index[j][sigs[j]].push_back(recind);
		}
	}
}

template<class Similarity>
LSH<Similarity>::~LSH() {
	SignaturesToCleanup::iterator it = signaturestocleanup.begin();
	for(; it != signaturestocleanup.end() ; ++it) {
		delete *it;
	}
}



#endif
