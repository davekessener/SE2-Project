#include "test/ut/fun_chain.h"

#include "test/unit/assertions.h"

#include "lib/chain.h"

namespace esep { namespace test { namespace ut {

template<int I> struct Add { int operator()(int&& i) { return i + I; } };
template<int I> struct Mul { int operator()(int&& i) { return i * I; } };

struct Counter
{
	Counter( ) { ++def; }
	Counter(const Counter&) { ++copy; }
	Counter(Counter&&) { ++move; }

	Counter& operator=(const Counter&) = delete;
	Counter& operator=(Counter&&) = delete;

	static int def;
	static int copy;
	static int move;
	static int exec;
};

int Counter::def = 0;
int Counter::copy = 0;
int Counter::move = 0;
int Counter::exec = 0;

struct Exec { Counter operator()(Counter&& c) { ++Counter::exec; return std::move(c); } };

FunChain::FunChain(void)
	: unit::TestSuite("Functor Chain")
{
}

void FunChain::define(void)
{
	UNIT_TEST("can execute")
	{
		typedef lib::FunChain<Add<3>, Mul<2>, Add<-1>> f;

		ASSERT_EQUALS(f()(1), 7);
	};

	UNIT_TEST("moves data")
	{
		Counter::def = 0;
		Counter::copy = 0;
		Counter::move = 0;
		Counter::exec = 0;

		typedef lib::FunChain<Exec, Exec, Exec, Exec, Exec> f;

		Counter c;

		Counter cc(f()(c));

		ASSERT_EQUALS(Counter::def, 1);
		ASSERT_EQUALS(Counter::copy, 1);
		ASSERT_EQUALS(Counter::move, 6);
		ASSERT_EQUALS(Counter::exec, 5);
	};
}

}}}
