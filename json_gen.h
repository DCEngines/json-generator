
#pragma once
#include <jansson.h>
#include <random>
#include <memory>
#include <glog/logging.h>
#include <cassert>


extern std::string get_sentence();

/* Base class for the generator. A generator can generate any of the
 * json_t types */
class Generator {
public:
	typedef std::shared_ptr<Generator> SPtr;
	virtual json_t* GetOne() = 0;
	static void loadFile(std::vector<std::string> &wordVec, std::string filename);
	virtual std::vector<json_t *> GetMany(int num) {
		std::vector<json_t *> result;
		for (auto cntr = 0; cntr < num; cntr++ ) {
			auto ret = GetOne();
			if(ret) {
				result.push_back(ret);
			}
		}
		return std::move(result);
	}
	virtual ~Generator() {}
	std::string GetString() {
		VLOG(2) << "in get string ";
		auto json = GetOne();
		if (json == nullptr) {
			VLOG(2) << "got null string ";
		}
		auto str = json_dumps(json, 2);
		VLOG(2) << "got this string " << str << json;
		std::string ret(str);
		std::free(str);
		json_decref(json);
		return ret;
	}
	void Seed(int seed) {
		gen.seed(seed);
	}
public:
	static std::mt19937 gen;
};

/* monotonic sequence number generator */
class SequenceNum : public Generator {
	uint64_t seq_;
	int      inc_;
public:
	SequenceNum(uint64_t startnum = 0, int increment = 1) {
		seq_ = startnum;
		inc_ = increment;
	}
	virtual json_t* GetOne() override {
		VLOG(2) << "in getone ";
		auto ret = json_integer(seq_);
		seq_ += inc_;
		return ret;
	}
};

class RandomInt : public Generator {
	std::uniform_int_distribution<uint64_t> rand;
public:
	RandomInt(uint64_t min, uint64_t max) : rand(min, max) {
	}
	virtual json_t* GetOne() override {
		return json_integer(rand(gen));
	}
};

class RandomReal : public Generator {
	std::uniform_real_distribution<double> rand;
public:
	RandomReal(double min, double max) : rand(min, max) {
	}
	virtual json_t* GetOne() override {
		return json_real(rand(gen));
	}
};

class Object : public Generator {
	std::map<const std::string, Generator::SPtr> children_;
public:
	virtual json_t* GetOne() override {
		auto retjson = json_object();
		VLOG(2) << "start or obj " << retjson;
		for(auto itr = children_.begin(); itr != children_.end(); ++itr) {
			auto json = itr->second->GetOne();
			if (json) {
				VLOG(2) << " adding to obj";
				json_object_set_new(retjson, itr->first.c_str(), json);
			}
		}
		if (json_object_size(retjson) == 0) {
			VLOG(2) << "got null json " << retjson;
			json_decref(retjson);
			retjson = nullptr;
		} else {
			VLOG(2) << "not null json " << retjson;
		}
		return retjson;
	}
	void addChild(const std::string &name, Generator::SPtr child) {
		auto itr = children_.find(name);
		assert(itr == children_.end());
		children_[name] = child;
	}
};

class Array : public Generator {
	Generator::SPtr count_;
	Generator::SPtr item_;
public:
	Array(Generator::SPtr count, Generator::SPtr item) {
		count_ = count;
		item_ = item;
	}
	virtual json_t* GetOne() override {
		auto retjson = json_array();
		auto count = count_->GetOne();
		assert(json_is_integer(count));
		auto cntr = json_integer_value(count);
		json_decref(count);
		while (cntr--) {
			json_array_append_new(retjson, item_->GetOne());
		}
		if (json_array_size(retjson) == 0) {
			json_decref(retjson);
			retjson = nullptr;
		}
		return retjson;
	}
};

class RandomLine : public Generator {
	std::vector<std::string> line_vec_;
	std::unique_ptr<std::uniform_int_distribution<uint64_t>> rand;
public:
	RandomLine(std::string file) {
		loadFile(line_vec_, file);
		assert(line_vec_.size() > 0);
		rand = std::make_unique<std::uniform_int_distribution<uint64_t>>(0, line_vec_.size() - 1);
	}
	virtual json_t* GetOne() override {
		return json_string(GetWord().c_str());
	}
	virtual std::string &GetWord() {
		auto num = rand->operator()(gen);
		assert(num < line_vec_.size());
		return line_vec_[num];
	}
};

class Names : public RandomLine {
public:
	Names() : RandomLine("../files/NAMES.TXT") {
	}
};

class Words : public RandomLine {
public:
	Words() : RandomLine("../files/SINGLE.TXT") {
	}
};

class Sentence : public Words {
	int length_chars_;
public:
	Sentence(int length_chars) {
		// length in characters
		length_chars_ = length_chars;
	}
	virtual json_t* GetOne() override {
		std::string sentence;
		sentence = GetWord();
		while (sentence.size() < length_chars_) {
			sentence.append(" " + GetWord());
		}
		return json_string(sentence.c_str());
	}
};

class Distribution : public Generator {
	std::map<double, Generator::SPtr> distribution_;
	Generator::SPtr default_gen_;
	double max_;
	std::uniform_real_distribution<double> rand;
public:
	Distribution(std::map<double, Generator::SPtr> distribution) {
		double cummulative = 0;
		for (auto &itr: distribution) {
			if (itr.first == 0.0) {
				default_gen_ = itr.second;
			}
			cummulative += itr.first;
			distribution_[cummulative] = itr.second;
		}
		// sum of probabilities can't be more than one
		assert(cummulative <= 1.0);
		// make sure a default generator is always specified
		assert(default_gen_);
		max_ = cummulative;
	}
	virtual json_t* GetOne() override {
		double value = rand(gen);
		Generator::SPtr gen;
		VLOG(1) << "got rand " << value;
		if (value > max_) {
			VLOG(1) << "above max " << max_ << " value " << value;
			gen = default_gen_;
		} else {
			auto itr = distribution_.upper_bound(value);
			assert(itr != distribution_.end());
			VLOG(1) << "got map value " << itr->first;
			gen = itr->second;
		}
		return gen->GetOne();
	}
};

class OneRandom : public Generator {
	json_t *the_one_ {nullptr};
public:
	OneRandom(Generator::SPtr gen) {
		the_one_ = gen->GetOne();
	}
	~OneRandom() {
		if (the_one_) {
			json_decref(the_one_);
		}
	}
	OneRandom(std::string str) {
		the_one_ = json_string(str.c_str());
	}
	virtual json_t* GetOne() override {
		return json_copy(the_one_);
	}
};

class NullJson : public Generator {
public:
	virtual json_t* GetOne() override {
		return nullptr;
	}
};

class ProperSentence : public Generator {
public:
	virtual json_t* GetOne() override {
		auto str = get_sentence();
		return json_string(str.c_str());
	}
};
