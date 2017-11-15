#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "classes.h"
#include "data.h"
#include "ppjoin.h"
#include "ppjoinpolicies.h"

typedef PPJoinAndPlus<PPJoinPolicy> PPJoin;

int main(void) {

	std::vector<IntRecord> records;

	{
		std::string line;
		std::ifstream inputfile;

		tokencountcounterhash tokencount;
		tokencount.set_empty_key(std::string(""));

		inputfile.open("/tmp/dblp1000.txt");
		if( inputfile.is_open()) {
			while (std::getline(inputfile, line)) {
				tokenvector tokens = tokenize_whitespace(line);
				update_counting(tokens, tokencount);
				tokenvector::iterator tokit = tokens.begin();
				line = "";
			}
			token2int(tokencount);
			inputfile.clear();
			inputfile.seekg(0);
			while (std::getline(inputfile, line)) {
				tokenvector tokens = tokenize_whitespace(line);
				collect_sets(records, tokens, tokencount);
				line = "";
			}
		} else {
			perror("could not open file");
			abort();
		}

		tokencountcounterhash::iterator tit = tokencount.begin();
		for( ; tit != tokencount.end(); ++tit) {
//			std::cout << tit->first.token << " " << tit->first.count << "," << tit->second << std::endl;
		}
	}
	Result result;

	PPJoin algo;
	algo.prepare(records);
	algo.run(records, 0.7, result);
	Result::iterator resit = result.begin();
	for( ; resit != result.end(); ++resit) {
		std::cout << " (" << resit->first << ", " << resit->second << ")" << std::endl;
	}
	
}


