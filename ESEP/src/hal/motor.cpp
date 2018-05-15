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
	mHAL->set(HAL::Field::GPIO_1, isGoingRight() ? MXT_BM_RIGHT : MXT_BM_LEFT);
	mRunning = true;
}

void Motor::stop(void)
{
	mHAL->reset(HAL::Field::GPIO_1, MXT_BM_LEFT | MXT_BM_RIGHT);
	mRunning = false;
}

void Motor::slow(void)
{
	mHAL->set(HAL::Field::GPIO_1, MXT_BM_SLOW);
	mFast = false;
}

void Motor::fast(void)
{
	mHAL->reset(HAL::Field::GPIO_1, MXT_BM_SLOW);
	mFast = true;
}

void Motor::enable(void)
{
	mHAL->reset(HAL::Field::GPIO_1, MXT_BM_DISABLE);
	mEnabled = true;
}

void Motor::disable(void)
{
	mHAL->set(HAL::Field::GPIO_1, MXT_BM_DISABLE);
	mEnabled = false;
}

void Motor::right(void)
{
	mHAL->reset(HAL::Field::GPIO_1, MXT_BM_LEFT);
	mHAL->set(HAL::Field::GPIO_1, MXT_BM_RIGHT);

	mRight = true;
}

void Motor::left(void)
{
	mHAL->reset(HAL::Field::GPIO_1, MXT_BM_RIGHT);
	mHAL->set(HAL::Field::GPIO_1, MXT_BM_LEFT);

	mRight = false;
}

}}

