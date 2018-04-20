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
	get<hal::Switch>().close();
	get<hal::Motor>().stop();

	delete mHAL;
}

void Impl::run(const lib::args_t& args)
{
	lib::Timer::instance().reset();

	typedef hal::HAL::Event Event;

	std::atomic<bool> running;

	running = true;

	MXT_LOG("Running ESEP Project in release mode!");

	get<hal::Lights>().turnOn(hal::Lights::Light::RED);

	mHAL->setCallback([this, &running](Event e) {
		switch(e)
		{
		case Event::LB_START:
			if(get<hal::LightBarriers>().isBroken(hal::LightBarriers::LightBarrier::LB_START))
			{
				get<hal::Lights>().turnOn(hal::Lights::Light::GREEN);
			}
			else
			{
				get<hal::Lights>().turnOff(hal::Lights::Light::GREEN);
			}
			break;
		case Event::BTN_STOP:
			running = false;
			break;
		default:
			MXT_LOG("Received an event.");
			break;
		}
	});

	while(running.load())
	{
		lib::Timer::instance().sleep(100);
	}

	MXT_LOG("Goodbye!");

	get<hal::Lights>().turnOff(hal::Lights::Light::RED);
}

}}

