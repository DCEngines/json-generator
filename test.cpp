#include <gtest/gtest.h>
#include <glog/logging.h>
#include "json_gen.h"

TEST(generator, basic)
{
	auto root_obj = std::make_shared<Object>();
	root_obj->Seed(10);
	auto seq = std::make_shared<SequenceNum>();
	root_obj->addChild("seq", seq);
	auto fname = std::make_shared<Names>();
	root_obj->addChild("fname",  fname);
	auto lname = std::make_shared<Names>();
	root_obj->addChild("lname", lname);
	auto word = std::make_shared<Words> ();
	root_obj->addChild("word", word);
	auto sentence = std::make_shared<ProperSentence>();
	root_obj->addChild("sentence", sentence);
	auto rand_sentence = std::make_shared<OneRandom>(sentence);
	root_obj->addChild("rand_fixed_sentence", rand_sentence);
	auto fixed = std::make_shared<OneRandom>("only string this one");
	root_obj->addChild("fixed_sentence", fixed);
	std::map<double, Generator::SPtr> distribution = {
		{0.1, std::make_shared<OneRandom>("heads")},
		{0.11, std::make_shared<OneRandom>("tails")},
		{0.5, std::make_shared<NullJson>()},
		{0, std::make_shared<OneRandom>("default")},
	};
	auto flip = std::make_shared<Distribution>(distribution);
	root_obj->addChild("coin_flip", flip);
	auto nest_obj = std::make_shared<Object>();
	nest_obj->addChild("Comment", std::make_shared<ProperSentence>());
	std::map<double, Generator::SPtr> dist = {
		{0.60, std::make_shared<OneRandom>("India")},
		{0, word},
	};
	nest_obj->addChild("country", std::make_shared<Distribution>(dist));
	root_obj->addChild("nested", nest_obj);
	auto array = std::make_shared<Array>(std::make_shared<RandomInt>(5,11),
										 std::make_shared<RandomReal>(-11.56, 20.55));
	root_obj->addChild("array_of_reals", array);
	int id = 0;
	std::cout << id++ << root_obj->GetString() << std::endl;
	std::cout << id++ << root_obj->GetString() << std::endl;
	std::cout << id++ << root_obj->GetString() << std::endl;
	std::cout << id++ << root_obj->GetString() << std::endl;
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	return RUN_ALL_TESTS();
}
