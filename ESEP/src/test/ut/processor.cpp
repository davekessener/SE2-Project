#include "test/ut/processor.h"

#include "test/unit/assertions.h"

#include "lib/processor.h"
#include "lib/process_tree.h"

namespace esep { namespace test { namespace ut {

namespace
{
	template<int I> struct Add { int operator()(const int& i) { return i + I; } };
	template<int I> struct Mul { int operator()(const int& i) { return i * I; } };

	struct Counter
	{
		int operator()(const int& i)
		{
			++count;

			return i;
		}

		static int count;
	};

	int Counter::count = 0;

	enum class Stat
	{
		S0,
		S1,
		S2
	};

	template<Stat S, typename T>
	using Tag = lib::data::Tagger<Stat>::Tag<S, T>;

	template<typename T, typename ... TT>
	using Source = lib::data::Source<T, TT...>;

	template<typename T, typename P>
	using Proc = lib::data::ProcessTree<Stat, T, P>;

	typedef Proc<int, Source<Counter, Source<Add<3>, Source<Mul<2>, Tag<Stat::S0, Add<-1>>>>>> proc_t;

	typedef lib::Processor<int, proc_t, 3> processor_t;
}

Processor::Processor(void)
	: unit::TestSuite("Processor")
{
}

void Processor::define(void)
{
	UNIT_TEST("can instantiate")
	{
		Counter::count = 0;

		processor_t p;
	};

	UNIT_TEST("can process")
	{
		Counter::count = 0;

		processor_t p;

		ASSERT_EQUALS(p.processor(1).get<Stat::S0>(), 7);
	};

	UNIT_TEST("stores processing trees")
	{
		Counter::count = 0;

		processor_t p;

		ASSERT_EQUALS(p.processor(3).get<Stat::S0>(), 11);
		ASSERT_EQUALS(p.processor(1).get<Stat::S0>(), 7);
		ASSERT_EQUALS(p.processor(3).get<Stat::S0>(), 11);

		ASSERT_EQUALS(Counter::count, 2);
	};
}

}}}
