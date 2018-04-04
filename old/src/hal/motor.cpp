#include "hal/motor.h"

#define MXT_BM_RUN		(1 << 13)
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
		mHAL->setBits(HAL::Port::A, MXT_BM_RUN);
		mRunning = true;
	}
}

void Motor::stop(void)
{
	if(isRunning())
	{
		mHAL->resetBits(HAL::Port::A, MXT_BM_RUN);
		mRunning = false;
	}
}

void Motor::slow(void)
{
	if(isFast())
	{
		mHAL->setBits(HAL::Port::A, MXT_BM_SLOW);
		mFast = false;
	}
}

void Motor::fast(void)
{
	if(!isFast())
	{
		mHAL->resetBits(HAL::Port::A, MXT_BM_SLOW);
		mFast = true;
	}
}

void Motor::enable(void)
{
	if(!isEnabled())
	{
		mHAL->setBits(HAL::Port::A, MXT_BM_DISABLE);
		mEnabled = true;
	}
}

void Motor::disable(void)
{
	if(isEnabled())
	{
		mHAL->resetBits(HAL::Port::A, MXT_BM_DISABLE);
		mEnabled = false;
	}
}

}}

