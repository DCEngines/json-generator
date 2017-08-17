#include <gtest/gtest.h>
#include <glog/logging.h>
#include "json_gen.h"

TEST(generator, basic)
{
	// first we create the root object
	auto root_obj = std::make_shared<Object>();
	root_obj->Seed(10);

	// lets add a auto incrementing sequence num
	auto seq = std::make_shared<SequenceNum>();
	// now add the sequence number as filed "seq" in root object
	root_obj->addChild("seq", seq);

	// random names
	auto fname = std::make_shared<Names>();
	// add as field "fname"
	root_obj->addChild("fname",  fname);

	auto lname = std::make_shared<Names>();
	// lname also picked randomly from names.
	root_obj->addChild("lname", lname);

	// a random word
	auto word = std::make_shared<Words> ();
	root_obj->addChild("word", word);

	// randomly generate a proper sentence
	auto sentence = std::make_shared<ProperSentence>();
	root_obj->addChild("sentence", sentence);

	// The generator OneRandom invokes the generator once and then
	// uses the same string for all subsequent calles. This is useful
	// in generating distributions. OneRandom can take a string or a
	// Generator::SPtr as pointer.
	auto rand_sentence = std::make_shared<OneRandom>(sentence);
	root_obj->addChild("rand_fixed_sentence", rand_sentence);

	auto fixed = std::make_shared<OneRandom>("only string this one");
	root_obj->addChild("fixed_sentence", fixed);

	// distribution take a map of probabilities and the
	// Generator::SPtr. The generator would be called with specified
	// probability. The sum of specified probabilities must be less
	// than 1.0 For the remainder of the time the default function is
	// called. The default is specified with a probability of zero(0).
	std::map<double, Generator::SPtr> distribution = {
		{0.1, std::make_shared<OneRandom>("heads")}, // occurs with probability of 0.1
		{0.11, std::make_shared<OneRandom>("tails")}, // occurs with probability 0.11
		// by specifying a NullJson you can create an optional field
		// which may or may not be present.
		{0.5, std::make_shared<NullJson>()}, // occurs with probabilty 0.5
		{0, std::make_shared<OneRandom>("default")}, // the rest of the time.
	};
	auto flip = std::make_shared<Distribution>(distribution);
	root_obj->addChild("coin_flip", flip);

	// you can have arbitrary level of nesting as all generators
	// including distributions are Generator:SPtr you can nest
	// distributions inside distributions if you like.
	auto nest_obj = std::make_shared<Object>();
	root_obj->addChild("nested", nest_obj);

	nest_obj->addChild("Comment", std::make_shared<ProperSentence>());

	std::map<double, Generator::SPtr> dist = {
		{0.60, std::make_shared<OneRandom>("India")},
		{0, word},
	};
	nest_obj->addChild("country", std::make_shared<Distribution>(dist));

	// arrays are specified by two parameters. The first is a
	// generator which generates an integer. The second is the
	// generator which will generate the values in the array.
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
