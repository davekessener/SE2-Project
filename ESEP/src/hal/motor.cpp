#include "hal/motor.h"

#define MXT_BM_RUN		(1 << 12)
#define MXT_BM_SLOW		(1 << 14)
#define MXT_BM_DISABLE	(1 << 15)

namespace esep { namespace hal {

Motor::Motor(HAL *hal)
	: mHAL(hal)
	, mRunning(false)
	, mFast(true)
	, mEnabled(true)
{
}

Motor::~Motor(void)
{
	if(isRunning())
	{
		stop();
	}

	if(!isFast())
	{
		fast();
	}

	if(!isEnabled())
	{
		enable();
	}
}

void Motor::start(void)
{
	if(!isRunning())
	{
		mHAL->set(HAL::Field::GPIO_1, MXT_BM_RUN);
		mRunning = true;
	}
}

void Motor::stop(void)
{
	if(isRunning())
	{
		mHAL->reset(HAL::Field::GPIO_1, MXT_BM_RUN);
		mRunning = false;
	}
}

void Motor::slow(void)
{
	if(isFast())
	{
		mHAL->set(HAL::Field::GPIO_1, MXT_BM_SLOW);
		mFast = false;
	}
}

void Motor::fast(void)
{
	if(!isFast())
	{
		mHAL->reset(HAL::Field::GPIO_1, MXT_BM_SLOW);
		mFast = true;
	}
}

void Motor::enable(void)
{
	if(!isEnabled())
	{
		mHAL->set(HAL::Field::GPIO_1, MXT_BM_DISABLE);
		mEnabled = true;
	}
}

void Motor::disable(void)
{
	if(isEnabled())
	{
		mHAL->reset(HAL::Field::GPIO_1, MXT_BM_DISABLE);
		mEnabled = false;
	}
}

}}

