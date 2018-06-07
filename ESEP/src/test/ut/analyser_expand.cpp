#include "test/ut/analyser_expand.h"

#include "test/unit/assertions.h"

#include "lib/stream/stream.h"
#include "lib/analyse/expand.h"

#include "lib/logger.h"

namespace esep { namespace test { namespace ut {

#define MXT_MUL 4

namespace
{
	typedef std::pair<int, int> value_type;
	typedef lib::stream::Stream<value_type> stream_t;
	typedef analyse::Expand<value_type> expand_t;
}

AnalyserExpand::AnalyserExpand(void)
	: unit::TestSuite("Analyser::Expand")
{
	for(int i = 0 ; i < (4 - 1) * MXT_MUL + 1 ; ++i)
	{
		int v = (i/MXT_MUL + 1) * (i/MXT_MUL + 1);

		mCmp.push_back(std::make_pair(i, v));
	}
}

void AnalyserExpand::setup(void)
{
	mBuf.clear();

	for(int i = 0 ; i < 4 ; ++i)
	{
		mBuf.push_back(std::make_pair(MXT_MUL * i, (i + 1) * (i + 1)));
	}
}

void AnalyserExpand::define(void)
{
	UNIT_TEST("can instantiate")
	{
		stream_t s(mBuf);

		expand_t i1(s), i2;
	};

	UNIT_TEST("expands streams")
	{
		stream_t s(mBuf);

		expand_t i1(std::move(s)), i2;

		std::vector<value_type> r;

		for(; i1 != i2 ; ++i1)
		{
			r.push_back(*i1);
		}

		ASSERT_EACH_EQUALS(r, mCmp);
	};

	UNIT_TEST("can be used in streams")
	{
		std::vector<value_type> r;

		stream_t s{expand_t{stream_t{mBuf}}, expand_t{}};

		s.collect([&](const value_type& e) { r.push_back(e); });

		ASSERT_EACH_EQUALS(r, mCmp);
	};
}

}}}
