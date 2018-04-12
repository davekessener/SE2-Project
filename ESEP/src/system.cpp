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
	typedef hal::LightBarriers::LightBarrier LightBarrier;
	typedef hal::Lights::Light Light;
	typedef hal::Buttons::Button Button;
	typedef hal::HAL::Event Event;

	auto& timer(lib::Timer::instance());

	timer.reset();

	hal::Motor& motor(get<hal::Motor>());
	hal::Lights& lights(get<hal::Lights>());
	hal::LightBarriers& lbs(get<hal::LightBarriers>());
	hal::Buttons& btns(get<hal::Buttons>());
	std::atomic<bool> running;
	uint c = 0;

	running = true;

	timer.sleep(100);

	lights.turnOn(Light::GREEN);

	mHAL->subscribeEvent(Event::BTN_START, [&](Event e) {
		if(btns.isPressed(Button::START))
		{
			lights.turnOn(Light::RED);
		}
		else
		{
			lights.turnOff(Light::RED);
		}

		++c;
	});

	mHAL->subscribeEvent(Event::BTN_STOP, [&](Event e) {
		running = false;
	});

	MXT_LOG("Hello, World!");
	MXT_LOG("Starting 'Aktorik-Test'!");

	while(running.load())
	{
		timer.sleep(200);
	}

	lights.turnOff(Light::RED);
	lights.turnOff(Light::GREEN);
	lights.turnOff(Light::YELLOW);
	motor.fast();
	motor.stop();
	motor.enable();

	MXT_LOG(lib::stringify("Received ", c, " START events."));

	MXT_LOG("Shutting down now, goodbye!");
}

}}

