#ifndef SSJ_SIMILARITY_H
#define SSJ_SIMILARITY_H

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

#include <cmath>
#include <iostream>

#define PMAXSIZE_EPS 1e-10

template <typename Similarity>
class GenericSimilarity {
	public:
		typedef typename Similarity::threshold_type threshold_type;

		inline static unsigned int maxprefix(unsigned int len, threshold_type threshold) {
			return std::min(len, len - minsize(len, threshold) + 1);
		}

		inline static unsigned int midprefix(unsigned int len, threshold_type threshold) {
			return std::min(len, len - minoverlap(len, len, threshold) + 1);
		}
		
		inline static unsigned int minoverlap(unsigned int len1, unsigned int len2, threshold_type threshold) {
			return std::min(len2, std::min(len1, Similarity::minoverlap(len1, len2, threshold)));
		}

		inline static unsigned int minsize(unsigned int len, threshold_type threshold) {
			return Similarity::minsize(len, threshold);
		}

		inline static unsigned int maxsize(unsigned int len, threshold_type threshold, unsigned int maxLen) {
			return Similarity::maxsize(len, threshold, maxLen);
		}

		inline static unsigned int maxsize(unsigned int len, unsigned int pos, threshold_type threshold,  unsigned int maxLen) {
			return Similarity::maxsize(len, pos, threshold, maxLen);
		}
		
};

class JaccardSimilarity {
	public:
		typedef double threshold_type;
		inline static unsigned int minoverlap(unsigned int len1, unsigned int len2, double threshold) {
			return (unsigned int)(ceil((len1 + len2) * threshold / (1 + threshold)));
		}
		
		inline static unsigned int minsize(unsigned int len, double threshold) {
			return (unsigned int)(ceil(threshold * len));
		}
		
		inline static unsigned int maxsize(unsigned int len, double threshold, unsigned int maxLen) {
			(void) maxLen;
			return (unsigned int)((len / threshold));
		}
		
		inline static unsigned int maxsize(unsigned int len, unsigned int pos, double threshold, unsigned int maxLen) {
			(void) maxLen;
			return (unsigned int)((len - ((1.0 - PMAXSIZE_EPS) + threshold) * pos) / threshold);
		}
		
};

class CosineSimilarity {
	public:
		typedef double threshold_type;
		inline static unsigned int minoverlap(unsigned int len1, unsigned int len2, double threshold) {
			std::cout<<"len1: "<<len1<<" len2: "<<len2<<std::endl;
			std::cout<<"minoverlap:"<<(unsigned int)ceil(threshold * len1)<<std::endl;
			return (unsigned int)ceil(threshold * len1);
		}
		
		inline static unsigned int minsize(unsigned int len, double threshold) {
			std::cout<<"len:"<<len<<std::endl;
			std::cout<<"minsize:"<<(unsigned int)(ceil(threshold * len))<<std::endl;
			return (unsigned int)(ceil(threshold * len));
		}
		
		inline static unsigned int maxsize(unsigned int len, double threshold, unsigned int maxLen) {
			std::cout<<len<< " max  "<<maxLen<<" threshold "<<threshold<<std::endl;
			return maxLen;
		}
		
		inline static unsigned int maxsize(unsigned int len, unsigned int pos, double threshold, unsigned int maxLen) {
			std::cout<<len<< " pos max  "<<maxLen<<" threshold "<<threshold<<std::endl;
			return maxLen;
		}
		
};

class DiceSimilarity {
	public:
		typedef double threshold_type;
		inline static unsigned int minoverlap(unsigned int len1, unsigned int len2, double threshold) {
			std::cout<<"len1: "<<len1<<" len2: "<<len2<<std::endl;
			std::cout<<"minoverlap:"<<(unsigned int)ceil(threshold * len1)<<std::endl;
			return (unsigned int)ceil(threshold * len1);
		}
		
		inline static unsigned int minsize(unsigned int len, double threshold) {
			std::cout<<"len:"<<len<<std::endl;
			std::cout<<"minsize:"<<(unsigned int)(ceil(threshold * len))<<std::endl;
			return 0;
		}
		
		inline static unsigned int maxsize(unsigned int len, double threshold, unsigned int maxLen) {
			std::cout<<len<< " max  "<<maxLen<<" threshold "<<threshold<<std::endl;
			return maxLen;
		}
		
		inline static unsigned int maxsize(unsigned int len, unsigned int pos, double threshold, unsigned int maxLen) {
			std::cout<<len<< " pos  max  "<<maxLen<<" threshold "<<threshold<<std::endl;
			return maxLen;
		}
		
};

class HammingSimilarity {
	public:
		typedef unsigned int threshold_type;
		inline static unsigned int minoverlap(unsigned int len1, unsigned int len2, threshold_type threshold) {
			//Ensure that minoverlap is at least 1
			if(len1 + len2 > threshold) {
				// the + 1 is there to avoid a float cast and a ceil on the whole term
				return (len1 + len2 - threshold + 1) / 2;
			} else {
				return 1;
			}
		}

		inline static unsigned int minsize(unsigned int len, threshold_type threshold) {
			return len > threshold ? len - threshold : 1;
		}

		inline static unsigned int maxsize(unsigned int len, threshold_type threshold, unsigned int maxLen) {
			(void) maxLen;
			return len + threshold;
		}

		inline static unsigned int maxsize(unsigned int len, unsigned int pos, threshold_type threshold, unsigned int maxLen) {
			(void) maxLen;
			return len + threshold - 2 * pos;
		}

};


typedef GenericSimilarity<JaccardSimilarity> Jaccard;
typedef GenericSimilarity<CosineSimilarity> Cosine;
typedef GenericSimilarity<DiceSimilarity> Dice;
typedef GenericSimilarity<HammingSimilarity> Hamming;

#endif

