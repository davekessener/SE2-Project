#include <chrono>
#include <thread>

#include "system.h"

#include "hal/physical.h"
#include "hal/lights.h"
#include "hal/motor.h"

namespace esep { namespace system {

Impl::Impl(void)
	: mHAL(new hal::Physical())
{
}

Impl::~Impl(void)
{
	delete mHAL;
}

void Impl::run(void)
{
	typedef hal::Lights::Light Light;

	hal::Lights lights(mHAL);
	hal::Motor motor(mHAL);

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

