#ifdef ESEP_TEST

#include "test/unit/assertions.h"

#include "test/ut/crc32.h"

namespace esep { namespace test { namespace unit {

CRC32::CRC32(void)
	: TestSuite("CRC32 Generator")
	, mCRCGen(nullptr)
{
}

void CRC32::setup(void)
{
	mCRCGen = new gen_t(lib::crc::DEFAULT_POLYNOMIAL);
}

void CRC32::teardown(void)
{
	delete mCRCGen;
}

void CRC32::define(void)
{
	UNIT_TEST("gives empty crc for 0 polynomial")
	{
		delete mCRCGen;
		mCRCGen = new gen_t(0);

		auto r = mCRCGen->encode("This is a test.");

		ASSERT_EQUALS(r, static_cast<gen_t::crc_t>(-1));
	};

	UNIT_TEST("is deterministic")
	{
		std::string s = "This is test.";

		auto r1 = mCRCGen->encode(s);

		mCRCGen->encode("Another test.");

		auto r2 = mCRCGen->encode(s);

		ASSERT_EQUALS(r1, r2);
	};

	UNIT_TEST("can calculate crc of sample string")
	{
		auto r = mCRCGen->encode("The quick brown fox jumps over the lazy dog");

		ASSERT_EQUALS(r, 0x8D462826u);
	};

	UNIT_TEST("differentiates crcs from different sized sources")
	{
		const char * const s = "The quick brown fox jumps over the lazy dog";
		auto r1 = mCRCGen->encode(s + 0, s + 10);
		auto r2 = mCRCGen->encode(s + 0, s + 11);

		ASSERT_NOT_EQUAL(r1, r2);
	};

	UNIT_TEST("can handle std::vector<byte_t>")
	{
		std::vector<byte_t> data{
			0x00, 0xFF, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3a,
			0x2f, 0x2f, 0x79, 0x6f, 0x75, 0x74, 0x75, 0x2e,
			0x62, 0x65, 0x2f, 0x78, 0x51, 0x77, 0x59, 0x7a,
			0x45, 0x65, 0x4d, 0x61, 0x69, 0x45, 0x00, 0xFF
		};

		auto r = mCRCGen->encode(data);

		ASSERT_EQUALS(r, 0x2D4A37EEu);
	};
}

}}}

#endif
