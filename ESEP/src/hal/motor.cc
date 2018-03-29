#include "hal/motor.h"

#define MXT_BM_RUN		0x01 // 0b00000001
#define MXT_BM_SLOW		0x04 // 0b00000100
#define MXT_BM_DISABLE	0x08 // 0b00001000

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
	}
}

void Motor::stop(void)
{
	if(isRunning())
	{
		mHAL->resetBits(HAL::Port::A, MXT_BM_RUN);
	}
}

void Motor::slow(void)
{
	if(isFast())
	{
		mHAL->setBits(HAL::Port::A, MXT_BM_SLOW);
	}
}

void Motor::fast(void)
{
	if(!isFast())
	{
		mHAL->resetBits(HAL::Port::A, MXT_BM_SLOW);
	}
}

void Motor::enable(void)
{
	if(!isEnabled())
	{
		mHAL->setBits(HAL::Port::A, MXT_BM_DISABLE);
	}
}

void Motor::disable(void)
{
	if(isEnabled())
	{
		mHAL->resetBits(HAL::Port::A, MXT_BM_DISABLE);
	}
}

}}

