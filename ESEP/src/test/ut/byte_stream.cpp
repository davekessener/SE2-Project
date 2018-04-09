#ifdef ESEP_TEST

#include "lib/byte_stream.h"

#include "test/unit/assertions.h"

#include "test/ut/byte_stream.h"

namespace esep { namespace test { namespace unit {

ByteStream::ByteStream(void)
	: TestSuite("ByteStream")
{
}

void ByteStream::define(void)
{
	UNIT_TEST("can instantiate")
	{
		lib::ByteStream bs;
	};

	UNIT_TEST("can write some data")
	{
		lib::ByteStream bs;

		bs << static_cast<uint64_t>(-1);

		ASSERT_EQUALS(bs.size(), 8u);
	};

	UNIT_TEST("stores data correctly")
	{
		lib::ByteStream bs;

		bs << static_cast<uint32_t>(0x12345678);

		uint32_t v = 0;

		bs >> v;

		ASSERT_EQUALS(bs.size(), 0u);
		ASSERT_EQUALS(v, 0x12345678u);
	};
}

}}}

#endif
