#include "lib/byte_stream.h"

#include "test/unit/assertions.h"

#include "test/ut/byte_stream.h"

namespace esep { namespace test { namespace ut {

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

	UNIT_TEST("can write whole buffers")
	{
		lib::ByteStream bs;
		std::string cmp("Hello, World!");

		bs << lib::byte_stream::insert_all(cmp);

		std::string s(bs.cbegin(), bs.cend());

		ASSERT_EQUALS(s, cmp);
	};

	UNIT_TEST("can write c-style array")
	{
		lib::ByteStream bs;
		const byte_t cmp[] = { 0xFF, 0x00, 0x12, 0x34, 0xFF };

		bs << lib::byte_stream::insert_all(cmp);

		std::vector<byte_t> r(bs.cbegin(), bs.cend());

		ASSERT_EACH_EQUALS(r, cmp);
	};

	UNIT_TEST("can write with iterator pair")
	{
		lib::ByteStream bs;
		const byte_t cmp[] = { 0xFF, 0x00, 0x12, 0x34, 0xFF };

		bs << lib::byte_stream::insert_all(cmp, cmp + sizeof(cmp));

		std::vector<byte_t> r(bs.cbegin(), bs.cend());

		ASSERT_EACH_EQUALS(r, cmp);
	};
}

}}}
