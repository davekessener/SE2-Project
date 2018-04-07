#include <chrono>
#include <thread>

#include "system.h"

#include "hal/physical.h"
#include "hal/lights.h"
#include "hal/motor.h"

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

void Impl::run(void)
{
	typedef hal::Lights::Light Light;

	hal::Lights& lights(get<hal::Lights>());
	hal::Motor& motor(get<hal::Motor>());

	motor.start();

	for(uint i = 0 ; i < 10 ; ++i)
	{
		lights.turnOn(Light::RED);
		sleep(500);
		lights.turnOff(Light::RED);
		sleep(500);
	}

	motor.stop();
}

void Impl::sleep(uint ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint64_t Impl::elapsed(void)
{
	return 0; // TODO
}

}}

