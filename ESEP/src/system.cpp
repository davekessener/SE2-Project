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
		hal::LightBarriers(mHAL),
		hal::Lights(mHAL),
		hal::MetalSensor(mHAL),
		hal::Motor(mHAL))
{
}

Impl::~Impl(void)
{
	delete mHAL;
}

void Impl::run(const lib::args_t& args)
{
	typedef hal::Lights::Light Light;

	auto& timer(lib::Timer::instance());

	timer.reset();

	hal::Lights& lights(get<hal::Lights>());
	hal::Motor& motor(get<hal::Motor>());

	MXT_LOG("Hello, World!");
	MXT_LOG("Starting 'Aktorik-Test'!");

	motor.start();

	for(uint i = 0 ; i < 10 ; ++i)
	{
		lights.turnOn(Light::RED);
		timer.sleep(500);
		lights.turnOff(Light::RED);
		timer.sleep(500);

		MXT_LOG(lib::stringify("Blinked for the ", (i + 1), "th time!"));
	}

	motor.stop();

	MXT_LOG("Shutting down now, goodbye!");
}

}}

