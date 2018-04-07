#include <chrono>
#include <thread>

#include "system.h"

#include "hal/physical.h"
#include "lib/logger.h"

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
	mSystemStart = std::chrono::system_clock::now();

	typedef hal::Lights::Light Light;

	hal::Lights& lights(get<hal::Lights>());
	hal::Motor& motor(get<hal::Motor>());

	MXT_LOG("Hello, World!");
	MXT_LOG("Starting 'Aktorik-Test'!");

	motor.start();

	for(uint i = 0 ; i < 10 ; ++i)
	{
		lights.turnOn(Light::RED);
		sleep(500);
		lights.turnOff(Light::RED);
		sleep(500);

		MXT_LOG(lib::stringify("Blinked for the ", (i + 1), "th time!"));
	}

	motor.stop();

	MXT_LOG("Shutting down now, goodbye!");
}

void Impl::sleep(uint ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

uint64_t Impl::elapsed(void)
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - mSystemStart).count();
}

}}

