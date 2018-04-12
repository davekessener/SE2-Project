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
	typedef hal::LightBarriers::LightBarrier LightBarrier;
	typedef hal::Lights::Light Light;
	typedef hal::Buttons::Button Button;
	typedef hal::HAL::Event Event;
	typedef hal::LEDs::LED LED;

	auto& timer(lib::Timer::instance());

	timer.reset();

	hal::Motor& motor(get<hal::Motor>());
	hal::Lights& lights(get<hal::Lights>());
	hal::LightBarriers& lbs(get<hal::LightBarriers>());
	hal::Buttons& btns(get<hal::Buttons>());
	hal::LEDs& leds(get<hal::LEDs>());
	hal::Switch& swtch(get<hal::Switch>());
	std::atomic<bool> running;

	running = true;

	timer.sleep(100);

	lights.turnOn(Light::GREEN);
	motor.start();

	leds.turnOn(LED::Q1);
	leds.turnOn(LED::Q2);
	leds.turnOn(LED::START);
	leds.turnOff(LED::RESET);

	mHAL->subscribeEvent(Event::BTN_START, [&](Event e) {
		if(btns.isPressed(Button::START))
		{
			lights.turnOn(Light::RED);
		}
		else
		{
			lights.turnOff(Light::RED);
		}
	});

	mHAL->subscribeEvent(Event::LB_START, [&](Event e) {
		if(lbs.isBroken(LightBarrier::LB_START))
		{
			motor.right();
		}
	});

	mHAL->subscribeEvent(Event::LB_END, [&](Event e) {
		if(lbs.isBroken(LightBarrier::LB_END))
		{
			motor.left();
		}
	});

	mHAL->subscribeEvent(Event::BTN_STOP, [&](Event e) {
		if(btns.isPressed(Button::STOP))
		{
			running = false;
		}
	});

	mHAL->subscribeEvent(Event::LB_SWITCH, [&](Event e) {
		if(motor.isGoingRight())
		{
			swtch.open();
		}
		else
		{
			swtch.close();
		}
	});

	MXT_LOG("Hello, World!");
	MXT_LOG("Starting 'Aktorik-Test'!");

	while(running.load())
	{
		timer.sleep(200);
	}

	leds.turnOff(LED::Q1);
	leds.turnOff(LED::Q2);
	leds.turnOff(LED::START);

	swtch.close();
	lights.turnOff(Light::RED);
	lights.turnOff(Light::GREEN);
	lights.turnOff(Light::YELLOW);
	motor.fast();
	motor.stop();
	motor.enable();

	MXT_LOG("Shutting down now, goodbye!");
}

}}

