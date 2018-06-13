#include "test/ut/any.h"

#include "test/unit/assertions.h"

#include "lib/any.h"

namespace esep { namespace test { namespace ut {

Any::Any(void)
	: unit::TestSuite("Any Object")
{
}

void Any::define(void)
{
	UNIT_TEST("can create")
	{
		lib::Any a;
	};

	UNIT_TEST("can assign some value")
	{
		lib::Any a;
		std::string t("Hello, World!");

		a = t;

		ASSERT_EQUALS(a.as<std::string>(), t);
	};

	UNIT_TEST("stops reading from empty")
	{
		lib::Any a;

		ASSERT_FAILURE(a.as<int>(), lib::Any::NoObjectException);
	};

	UNIT_TEST("stops reading with incorrect type")
	{
		lib::Any a(7);

		ASSERT_FAILURE(a.as<std::string>(), lib::Any::InvalidTypeException);
	};

	UNIT_TEST("can assign to each other")
	{
		lib::Any a(7);

		lib::Any b;

		b = a;

		ASSERT_EQUALS(a.as<int>(), b.as<int>());
	};
}

}}}
