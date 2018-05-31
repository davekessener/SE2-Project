#include "test/ut/compound_enum.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace ut {

CompoundEnum::CompoundEnum(void)
	: TestSuite("Compound Enum")
{
}

void CompoundEnum::define(void)
{
	UNIT_TEST("can create an object")
	{
		enum_t e = Small::A;

		ASSERT_TRUE(e == Small::A);
	};

	UNIT_TEST("basic equality")
	{
		enum_t e = Small::A;

		ASSERT_TRUE(e == Small::A);
		ASSERT_TRUE(e != Small::B);
		ASSERT_TRUE(e != Large::A);
	};

	UNIT_TEST("casting")
	{
		enum_t e = Small::A;

		Small t = e.as<Small>();

		ASSERT_TRUE(e.is<Small>());
		ASSERT_TRUE(!e.is<Large>());
		ASSERT_TRUE(t == Small::A);
	};

	UNIT_TEST("invalid casts fail")
	{
		enum_t e = Small::A;

		ASSERT_FAILURE(e.as<Large>(), enum_t::InvalidCastException);
	};

	UNIT_TEST("value independence")
	{
		enum_t e = Other::D;

		ASSERT_TRUE(e == Other::D);
		ASSERT_TRUE(e != Small::D);
		ASSERT_TRUE(e != Large::F);
	};
}

}}}
