#include "test/ut/process_tree.h"

#include "test/unit/assertions.h"

#include "lib/process_tree.h"

#include "lib/logger.h"

namespace esep { namespace test { namespace ut {

enum class Station
{
	S0,
	S1,
	S2
};


template<int I>
struct Mul
{
	int operator()(const int& v) { ++counter; return v * I; }
	static int counter;
};

template<int I>
int Mul<I>::counter = 0;

template<int I>
struct Add
{
	int operator()(const int& v) { return v + I; }
};

struct ToString
{
	std::string operator()(const int& i) { return lib::lexical_cast<std::string>(i); }
};

struct ToInt
{
	int operator()(const std::string& s) { return lib::lexical_cast<int>(s); }
};

ProcessTree::ProcessTree(void)
	: unit::TestSuite("Process Tree")
{
}

template<Station V, typename T>
using Tag = lib::data::Tagger<Station>::Tag<V, T>;

template<typename T, typename ... TT>
using Source = lib::data::Source<T, TT...>;

template<typename D, typename T>
using Process = lib::data::ProcessTree<Station, D, T>;

void ProcessTree::define(void)
{
	UNIT_TEST("can handle single process")
	{
		typedef Process<int, Tag<Station::S0, Add<1>>> p_t;

		p_t p(7);

		ASSERT_EQUALS(p.get<Station::S0>(), 8);
	};

	UNIT_TEST("can handle process sequence")
	{
		typedef Process<int,
			Source<
				Add<5>,
				Source<
					Mul<3>,
					Source<
						Add<-1>,
						Tag<Station::S0, Mul<7>>>>>>
		p_t;

		p_t p(-3);

		ASSERT_EQUALS(p.get<Station::S0>(), 35);
	};

	UNIT_TEST("can handle branches")
	{
		typedef Process<int,
			Source<
				Add<5>,
				Source<
					Mul<2>,
					Tag<Station::S0, Add<1>>>,
				Tag<Station::S1, Add<7>>>>
		p_t;

		p_t p(-2);

		ASSERT_EQUALS(p.get<Station::S0>(), 7);
		ASSERT_EQUALS(p.get<Station::S1>(), 10);
	};

	UNIT_TEST("can retrieve intermediate results")
	{
		typedef Process<int,
			Source<
				Tag<Station::S0, Add<5>>,
				Source<Mul<2>,
					Tag<Station::S1, Add<1>>>>>
		p_t;

		p_t p(1);

		ASSERT_EQUALS(p.get<Station::S1>(), 13);
		ASSERT_EQUALS(p.get<Station::S0>(), 6);
	};

	UNIT_TEST("stores intermediate result")
	{
		typedef Process<int,
			Source<
				Add<3>,
				Source<
					Tag<Station::S0, Mul<2>>,
					Tag<Station::S1, Add<1>>,
					Tag<Station::S2, Add<-1>>>>>
		p_t;

		Mul<2>::counter = 0;

		p_t p(-1);

		ASSERT_EQUALS(Mul<2>::counter, 0);
		ASSERT_EQUALS(p.get<Station::S1>(), 5);
		ASSERT_EQUALS(Mul<2>::counter, 1);
		ASSERT_EQUALS(p.get<Station::S2>(), 3);
		ASSERT_EQUALS(Mul<2>::counter, 1);
		ASSERT_EQUALS(p.get<Station::S0>(), 4);
		ASSERT_EQUALS(Mul<2>::counter, 1);
	};

	UNIT_TEST("can process different types")
	{
		typedef Process<std::string,
			Source<
				ToInt,
				Source<
					Mul<2>,
					Tag<Station::S0, ToString>,
					Source<
						Add<1>,
						Tag<Station::S1, ToString>>>>>
		p_t;

		p_t p("7");

		ASSERT_EQUALS(p.get<Station::S0>(), "14");
		ASSERT_EQUALS(p.get<Station::S1>(), "15");
	};
}

}}}
