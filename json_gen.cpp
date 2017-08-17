#include <gtest/gtest.h>
#include <glog/logging.h>

#include "json_gen.h"
#include <fstream>
#include <cassert>

std::random_device rd;
std::mt19937 Generator::gen(rd());


void Generator::loadFile(std::vector<std::string> &wordVec, std::string file_name)
{
	std::ifstream file(file_name, std::ios::in);
	assert(file.is_open());
	std::string line;
	while (std::getline(file, line)) {
		if (line.size() > 0) {
			while (line[line.size() - 1] == '\r' ||
				line[line.size() - 1] == '\n') {
				line.pop_back();
			}
			wordVec.emplace_back(line);
		}
	}
}
