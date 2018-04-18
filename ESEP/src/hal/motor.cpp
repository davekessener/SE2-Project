#include "hal/motor.h"

#define MXT_BM_LEFT		(1 << 13)
#define MXT_BM_RIGHT	(1 << 12)
#define MXT_BM_SLOW		(1 << 14)
#define MXT_BM_DISABLE	(1 << 15)

namespace esep { namespace hal {

Motor::Motor(HAL *hal)
	: mHAL(hal)
	, mRunning(false)
	, mFast(true)
	, mEnabled(true)
	, mRight(true)
{
}

Motor::~Motor(void)
{
}

void Motor::start(void)
{
	if(!isRunning())
	{
		mHAL->set(HAL::Field::GPIO_1, isGoingRight() ? MXT_BM_RIGHT : MXT_BM_LEFT);
		mRunning = true;
	}
}

void Motor::stop(void)
{
	if(isRunning())
	{
		mHAL->reset(HAL::Field::GPIO_1, MXT_BM_LEFT | MXT_BM_RIGHT);
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

void Motor::right(void)
{
	if(!isGoingRight())
	{
		if(isRunning())
		{
			mHAL->reset(HAL::Field::GPIO_1, MXT_BM_LEFT);
			mHAL->set(HAL::Field::GPIO_1, MXT_BM_RIGHT);
		}

		mRight = true;
	}
}

void Motor::left(void)
{
	if(isGoingRight())
	{
		if(isRunning())
		{
			mHAL->reset(HAL::Field::GPIO_1, MXT_BM_RIGHT);
			mHAL->set(HAL::Field::GPIO_1, MXT_BM_LEFT);
		}

		mRight = false;
	}
}

}}

