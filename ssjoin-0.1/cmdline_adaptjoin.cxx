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

#include <boost/mpl/vector/vector10.hpp>

#include "cmdline.h"

#include "adaptjoin.h"

#include "template_unroll.h"


namespace {

	using boost::mpl::vector;
	using boost::mpl::vector2;
	using boost::mpl::vector3;
	using boost::mpl::vector4;

	typedef vector4<Cosine, Dice, Hamming, Jaccard> Similarities;
	typedef vector3<AdaptJoinIndexOnTheFlyPolicy, AdaptJoinIndexFirstPolicy<false>, AdaptJoinIndexFirstPolicy<true> > AdaptJoinIndexingStategies;
	typedef vector2<AdaptJoinIndexStructurePolicy, AdaptJoinIndexMaxExtStructurePolicy<InvIndexVector> > AdaptJoinIndexingStructures;

	struct Execute {
		struct algoParams {
			double threshold;
		};
		template <class Class1, class Class2, class Class3>
			static Algorithm * get_algo( 
					std::vector<bool> & d1Pattern,
					std::vector<bool> & d2Pattern, 
					std::vector<bool> & d3Pattern,
					unsigned int c1distance,
					unsigned int c2distance,
					unsigned int c3distance,
					algoParams algo_params) {


				if(d1Pattern[c1distance] && d2Pattern[c2distance] && d3Pattern[c3distance]) {
					return new AdaptJoin<Class1, Class2, Class3 >(algo_params.threshold);
				} else {
					return NULL;
				}
			}
	};
}


Algorithm * adaptjoin_cmd_line(boost::program_options::variables_map & vm) {
	std::vector<bool> similarityPattern;
	std::vector<bool> adaptJoinIndexingStrategyPattern;
	std::vector<bool> adaptJoinIndexingStructurePattern;

	similarityPattern.resize(4);
	adaptJoinIndexingStrategyPattern.resize(3);
	adaptJoinIndexingStructurePattern.resize(2);

	std::string cmd_algo = vm["algorithm"].as<std::string>();

	if(vm.count("cosine")) {
		similarityPattern[0] = true;
	} else if (vm.count("dice")) {
		similarityPattern[1] = true;
	} else if (vm.count("hamming")) {
		similarityPattern[2] = true;
	} else {
		similarityPattern[3] = true;
	}


	if(vm.count("foreign-linewise")) {
		adaptJoinIndexingStrategyPattern[2] = 1;
	} else if(vm.count("indexfirst")) {
		adaptJoinIndexingStrategyPattern[1] = 1;
	} else {
		adaptJoinIndexingStrategyPattern[0] = 1;
	}
	
	if(vm.count("allprefext")) {
		adaptJoinIndexingStructurePattern[0] = 1;
	} else {
		adaptJoinIndexingStructurePattern[1] = 1;
	}

	double threshold = vm["threshold"].as<double>();

	Execute::algoParams algo_params;
	algo_params.threshold = threshold;

	return algo_template_unroll_d3::combine<
		Similarities,
		AdaptJoinIndexingStategies,
		AdaptJoinIndexingStructures,
		Execute>::Generate<>::get_algo(
				similarityPattern,
				adaptJoinIndexingStrategyPattern,
				adaptJoinIndexingStructurePattern,
				algo_params);

}
