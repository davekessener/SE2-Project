#include "test/unit/hal.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace unit {

TestHAL::TestHAL(void)
{
	for(uint i = 0 ; i < hal::HAL::N_FIELDS ; ++i)
	{
		mBuffer[i] = 0;
	}
}

uint32_t TestHAL::in(Field f)
{
	mReads.emplace_back(lib::Timer::instance().elapsed(), f, getField(f));

	return getField(f);
}

void TestHAL::out(Field f, uint32_t v)
{
	mWrites.emplace_back(lib::Timer::instance().elapsed(), f, v);

	setField(f, v);
}

void TestHAL::clear(void)
{
	mReads.clear();
	mWrites.clear();
	mCallback = nullptr;
}

void TestHAL::trigger(Event e)
{
	if(!static_cast<bool>(mCallback))
	{
		MXT_THROW_EX(MissingCallbackException);
	}

	mCallback(e);
}

}}}
