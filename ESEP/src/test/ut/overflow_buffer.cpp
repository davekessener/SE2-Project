#include "test/ut/overflow_buffer.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace ut {

OverflowBuffer::OverflowBuffer(void)
	: unit::TestSuite("Overflow Buffer")
{
	mBuf = nullptr;
}

void OverflowBuffer::setup(void)
{
	mBuf = new buf_t;
}

void OverflowBuffer::teardown(void)
{
	delete mBuf; mBuf = nullptr;
}

void OverflowBuffer::define(void)
{
	UNIT_TEST("can be created and destroyed")
	{
		ASSERT_EQUALS(mBuf->capacity(), 10u);
		ASSERT_EQUALS(mBuf->size(), 0u);
		ASSERT_TRUE(mBuf->empty());
		ASSERT_FALSE(mBuf->full());
	};

	UNIT_TEST("can handle few elements")
	{
		mBuf->insert(3);
		mBuf->insert(4);
		mBuf->insert(5);

		ASSERT_EQUALS(mBuf->capacity(), 10u);
		ASSERT_EQUALS(mBuf->size(), 3u);
		ASSERT_FALSE(mBuf->empty());
		ASSERT_FALSE(mBuf->full());
	};

	UNIT_TEST("can be filled")
	{
		for(uint i = 0 ; i < mBuf->capacity() ; ++i)
		{
			mBuf->insert(i);
		}

		ASSERT_EQUALS(mBuf->capacity(), 10u);
		ASSERT_EQUALS(mBuf->size(), 10u);
		ASSERT_FALSE(mBuf->empty());
		ASSERT_TRUE(mBuf->full());
	};

	UNIT_TEST("stores values")
	{
		for(uint i = 0 ; i < mBuf->capacity() ; ++i)
		{
			mBuf->insert((i + 1) * 3);
		}

		int i = 0;
		for(auto i1 = mBuf->begin(), i2 = mBuf->end() ; i1 != i2 ; ++i1)
		{
			ASSERT_EQUALS(*i1, 3 * ++i);
		}

		ASSERT_EQUALS(i, 10);
	};

	UNIT_TEST("overwrites values")
	{
		for(uint i = 0 ; i < mBuf->capacity() ; ++i)
		{
			mBuf->insert(i);
		}

		mBuf->insert(42);
		mBuf->insert(413);
		mBuf->insert(7);

		ASSERT_EQUALS(mBuf->size(), 10u);

		std::vector<int> cmp{3, 4, 5, 6, 7, 8, 9, 42, 413, 7};

		ASSERT_EACH_EQUALS(*mBuf, cmp);
	};

	UNIT_TEST("does not iterate over empty buffer")
	{
		ASSERT_TRUE(mBuf->empty());
		ASSERT_TRUE(mBuf->begin() == mBuf->end()); // TODO
	};
}

}}}
