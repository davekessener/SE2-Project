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
	typedef hal::Lights::Light Light;
//	typedef hal::LightBarriers::LightBarrier LightBarrier;

	auto& timer(lib::Timer::instance());

	timer.reset();

	hal::Lights& lights(get<hal::Lights>());
	hal::Motor& motor(get<hal::Motor>());
//	hal::LightBarriers& lbs(get<hal::LightBarriers>());

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
//		MXT_LOG(lib::stringify("LB_Start is now ", lbs.isBroken(LightBarrier::LB_START)));
	}

	motor.stop();

	MXT_LOG("Shutting down now, goodbye!");
}

}}

