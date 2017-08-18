#include <gflags/gflags.h>
#include <glog/logging.h>
#include "json_gen.h"
#include <iostream>
#include <limits>

/*
 * Data will have the following schema
 * {
 *   sequence:  //monotonically increasing sequence number
 *   uuid:      // randomly generated 64 bit number
 *   comment:   // sentence with frequency distribution of 0.01 i.e. only 100 unique comments
 *   pincode:   // distribution 400000 to 400500
 *   firstname: // random name
 *   lastname:  // random name
 * }
 */

Generator::SPtr data_generator()
{
	// first we create the root object
	auto root_obj = std::make_shared<Object>();
	// seed it so we always get the same data
	root_obj->Seed(10);

	// add the sequence number
	root_obj->addChild("sequence", std::make_shared<SequenceNum>());

	// add the random uuid
	root_obj->addChild("uuid", std::make_shared<RandomInt>(0, std::numeric_limits<uint64_t>::max()));

	// add the comments as per distribution
	double cumulative = 0;
	std::vector<std::pair<double, Generator::SPtr>> distribution;
	while (cumulative < 1) {
		double val = 0.01;
		if (cumulative == 0) { // first time set val as zero (default)
			val = 0;
		}
		distribution.push_back(std::make_pair(val, std::make_shared<OneRandom>(std::make_shared<ProperSentence>())));
		cumulative += 0.01;
	}
	root_obj->addChild("comment", std::make_shared<Distribution>(distribution));

	// add pin codes in pune
	root_obj->addChild("pincode", std::make_shared<RandomInt>(400000, 400500));

	auto name = std::make_shared<Names>();
	root_obj->addChild("firstname", name);
	root_obj->addChild("lastname", name);
	return root_obj;
}

constexpr uint64_t kTargetSize = 1 << 30; // 1 GB


int main(int argc, char *argv[])
{
	gflags::ParseCommandLineFlags(&argc, &argv, true);

	auto gen = data_generator();
	uint64_t size = 0;

	while (size < kTargetSize) {
		std::string one_row = gen->GetString();
		size += one_row.size();
		std::cout << one_row;
	}
}
