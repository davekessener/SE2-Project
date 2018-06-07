#include "test/ut/stream.h"

#include "test/unit/assertions.h"

#include "lib/stream/stream.h"

namespace esep { namespace test { namespace ut {

namespace
{
	typedef lib::stream::Stream<double> stream_t;
}

Stream::Stream(void)
	: unit::TestSuite("Stream")
{
}

void Stream::setup(void)
{
	mBuf.clear();

	for(uint i = 0 ; i < 10 ; ++i)
	{
		mBuf.push_back(i / 2.0);
	}
}

void Stream::define(void)
{
	UNIT_TEST("can instantiate")
	{
		stream_t s(mBuf);
	};

	UNIT_TEST("can collect")
	{
		stream_t s(mBuf);

		double r = 0.0;

		s.collect([&r](double v) { r += v; });

		ASSERT_EQUALS(r, 22.5);
	};

	UNIT_TEST("can reduce")
	{
		stream_t s(mBuf);

		ASSERT_EQUALS(s.reduce([](double a, double b) { return a + b; }, 0.0), 22.5);
	};

	UNIT_TEST("handles additional values in front")
	{
		stream_t s(mBuf);

		s.push_front(2.5);

		double r = 0.0, first;
		bool f = true;

		s.collect([&](double v) {
			r += v;
			if(f) first = v;
			f = false;
		});

		ASSERT_EQUALS(r, 25.0);
		ASSERT_EQUALS(first, 2.5);
	};

	UNIT_TEST("handles additional values in back")
	{
		stream_t s(mBuf);

		s.push_back(2.5);

		double r = 0.0, last;

		s.collect([&](double v) { r += (last = v); });

		ASSERT_EQUALS(r, 25.0);
		ASSERT_EQUALS(last, 2.5);
	};

	UNIT_TEST("can map values")
	{
		stream_t s(mBuf);

		s.push_front(9.0);
		s.push_back(11.0);

		std::vector<double> r;
		std::vector<double> cmp;

		cmp.push_back(9.0 * 9.0);
		for(const auto& e : mBuf) cmp.push_back(e * e);
		cmp.push_back(11.0 * 11.0);

		s.map([](double v) { return v * v; }).collect([&](double v) { r.push_back(v); });

		ASSERT_EACH_EQUALS(r, cmp);
	};
}

}}}
