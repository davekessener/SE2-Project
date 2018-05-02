#include <atomic>

#include "system.h"

#include "hal/physical.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "base/dummy_master.h"
#include "base/handler.h"

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
	get<hal::Switch>().close();
	get<hal::Motor>().stop();

	delete mHAL;
}

void Impl::run(const lib::args_t& args)
{
	lib::Timer::instance().reset();

	base::DummyMaster master;
	base::Handler handler(&master);

	mHAL->setCallback([&handler](hal::HAL::Event e) { handler.handle(e); });

	while(master.running())
	{
		lib::Timer::instance().sleep(10);
	}
}

}}

