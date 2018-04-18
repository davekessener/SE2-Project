#include <atomic>

#include "system.h"

#include "hal/physical.h"
#include "lib/logger.h"
#include "lib/timer.h"

namespace esep { namespace system {

Impl::Impl(void)
	: mHAL(new hal::Physical())
	, mHALObjects(
		hal::Buttons(mHAL),
		hal::HeightSensor(mHAL),
		hal::LEDs(mHAL),
		hal::Switch(mHAL),
		hal::LightBarriers(mHAL),
		hal::Lights(mHAL),
		hal::MetalSensor(mHAL),
		hal::Motor(mHAL))
{
#ifdef ESEP_TEST
	MXT_THROW("Instantiated system in test build!");
#endif
}

Impl::~Impl(void)
{
	delete mHAL;
}

void Impl::run(const lib::args_t& args)
{
}

}}

