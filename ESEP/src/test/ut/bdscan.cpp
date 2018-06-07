#include <vector>

#include "test/ut/bdscan.h"

#include "test/unit/assertions.h"

#include "lib/analyse/v.h"
#include "lib/analyse/bdscan.h"

namespace esep { namespace test { namespace ut {

namespace
{
	typedef analyse::V<double, 2> vec2;
	typedef analyse::BDSCAN<vec2> scan_t;
}

BDSCAN::BDSCAN(void)
	: unit::TestSuite("BDSCAN")
{
}

void BDSCAN::define(void)
{
	UNIT_TEST("can instantiate")
	{
		scan_t bdscan(2.0, 3);
	};

	UNIT_TEST("can find a cluster")
	{
		std::vector<vec2> data;

		data.emplace_back(0.0, 0.0);
		data.emplace_back(1.0, 0.0);
		data.emplace_back(1.0, 1.0);
		data.emplace_back(0.0, 1.0);

		scan_t bdscan(2.0, 3);

		auto r = bdscan(data);

		ASSERT_EQUALS(r.size(), 1u);
		ASSERT_EQUALS(r[0][0], 0.5);
		ASSERT_EQUALS(r[0][1], 0.5);
	};

	UNIT_TEST("ignores noise")
	{
		std::vector<vec2> data;

		data.emplace_back(0.0, 0.0);
		data.emplace_back(1.0, 0.0);
		data.emplace_back(1.0, 1.0);
		data.emplace_back(0.0, 1.0);

		data.emplace_back( 10.0,  10.0);
		data.emplace_back(-10.0,  10.0);
		data.emplace_back(-10.0, -10.0);
		data.emplace_back( 10.0, -10.0);

		scan_t bdscan(2.0, 3);

		auto r = bdscan(data);

		ASSERT_EQUALS(r.size(), 1u);
		ASSERT_EQUALS(r[0][0], 0.5);
		ASSERT_EQUALS(r[0][1], 0.5);
	};

	UNIT_TEST("multiple cluster")
	{
		std::vector<vec2> data;

		data.emplace_back(0.0, 0.0);
		data.emplace_back(1.0, 0.0);
		data.emplace_back(1.0, 1.0);
		data.emplace_back(0.0, 1.0);

		data.emplace_back( 10.0,  10.0);
		data.emplace_back(-10.0,  10.0);
		data.emplace_back(-10.0, -10.0);
		data.emplace_back( 10.0, -10.0);

		data.emplace_back(100, 100);
		data.emplace_back(101, 100);
		data.emplace_back(101, 101);
		data.emplace_back(100, 101);

		data.emplace_back(50, 50);
		data.emplace_back(51, 50);
		data.emplace_back(51, 51);
		data.emplace_back(50, 51);

		scan_t bdscan(2.0, 3);

		auto r = bdscan(data);

		ASSERT_EQUALS(r.size(), 3u);
	};
}

}}}
