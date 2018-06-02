#include "test/unit/hal.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace unit {

TestHAL::TestHAL(void)
{
	resetFields();
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
	resetFields();
}

void TestHAL::trigger(Event e)
{
	if(!static_cast<bool>(mCallback))
	{
		MXT_THROW_EX(MissingCallbackException);
	}

	mCallback(e);
}

void TestHAL::resetFields(void)
{
	setField(Field::GPIO_0,
		  HAL::getPin(Event::LB_START)
		| HAL::getPin(Event::LB_HEIGHTSENSOR)
		| HAL::getPin(Event::LB_SWITCH)
		| HAL::getPin(Event::LB_RAMP)
		| HAL::getPin(Event::LB_END)
		| HAL::getPin(Event::BTN_STOP)
		| HAL::getPin(Event::BTN_ESTOP)
	);

	setField(Field::GPIO_1, 0);
	setField(Field::GPIO_2, 0);
	setField(Field::ANALOG, 0);
}

}}}
