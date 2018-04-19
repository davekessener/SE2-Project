#include <iostream>

#include "test/ft/hal.h"

#include "lib/deferred.h"

#include "hal/physical.h"

namespace esep { namespace test { namespace functional {

HALTester::HALTester(void)
	: mHAL(new hal::Physical)
, BUTTONS(mHAL)
, HEIGHT_SENSOR(mHAL)
, LEDS(mHAL)
, SWITCH(mHAL)
, LIGHT_BARRIERS(mHAL)
, LIGHTS(mHAL)
, METAL_SENSOR(mHAL)
, MOTOR(mHAL)
{
	mTests.push_back(&HALTester::t_004);
	mTests.push_back(&HALTester::t_005);
	mTests.push_back(&HALTester::t_006);
	mTests.push_back(&HALTester::t_007);
	mTests.push_back(&HALTester::t_008);
	mTests.push_back(&HALTester::t_009);
	mTests.push_back(&HALTester::t_010);
	mTests.push_back(&HALTester::t_011);
}

HALTester::~HALTester(void)
{
	delete mHAL;
}

void HALTester::run(void)
{
	auto no_handler = [](Event e) { };
	std::string line;

	resetHAL();

	std::cout << "Starting interactive HAL test.\n"
			  << "===========================================================================================\n";

	for(uint i = 0 ; i < mTests.size() ; ++i)
	{
		lib::Deferred cleanup([this](void) { resetHAL(); clearTimers(); });
		test_fn f = mTests[i];

		std::cout << "Running test T-" << std::setw(3) << std::setfill('0') << (i + 4) << "\n"
				  << "Press ENTER to begin ...";

		std::getline(std::cin, line);

		mHAL->setCallback([this, f](Event e) { (this->*f)(e); });

		std::cout << "[running] ... (press ENTER to stop)";

		std::getline(std::cin, line);

		mHAL->setCallback(no_handler);

		std::cout << "\n--- END OF TEST ---------------------------------------------------------------------------\n\n";
	}

	std::cout << "===========================================================================================\n"
			  << "Shutting down interactive HAL test." << std::endl;
}

// # -----------------------------------------------------------------------------

void HALTester::clearTimers(void)
{
	for(const auto& id : mTimers)
	{
		lib::Timer::instance().unregisterCallback(id);
	}

	mTimers.clear();
}

void HALTester::resetHAL(void)
{
	SWITCH.close();
	MOTOR.right();
	MOTOR.fast();
	MOTOR.enable();
	MOTOR.stop();
	LEDS.turnOff(LED::START);
	LEDS.turnOff(LED::RESET);
	LEDS.turnOff(LED::Q1);
	LEDS.turnOff(LED::Q2);
	LIGHTS.turnOff(Light::RED);
	LIGHTS.turnOff(Light::YELLOW);
	LIGHTS.turnOff(Light::GREEN);
}

// # -----------------------------------------------------------------------------

void HALTester::t_004(Event e)
{
	switch(e)
	{
	case Event::BTN_START:
		if(BUTTONS.isPressed(Button::START))
		{
			LEDS.turnOn(LED::START);
			LIGHTS.turnOn(Light::GREEN);

			runIn(3000, [this](void) -> bool {
				LIGHTS.turnOff(Light::GREEN);

				return false;
			});
		}
		break;
	default:
		break;
	}
}

void HALTester::t_005(Event e)
{
	switch(e)
	{
	case Event::BTN_STOP:
		if(BUTTONS.isPressed(Button::STOP))
		{
			LIGHTS.turnOn(Light::RED);

			runIn(3000, [this](void) -> bool {
				LIGHTS.turnOff(Light::RED);

				return false;
			});
		}
		break;
	default:
		break;
	}
}

void HALTester::t_006(Event e)
{
	switch(e)
	{
	case Event::LB_START:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			MOTOR.start();
		}
		else
		{
			MOTOR.stop();
		}
		break;
	default:
		break;
	}
}

void HALTester::t_007(Event e)
{
	switch(e)
	{
	case Event::LB_END:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			MOTOR.left();
			MOTOR.start();
		}
		else
		{
			MOTOR.stop();
		}
		break;
	default:
		break;
	}
}

void HALTester::t_008(Event e)
{
	enum class State
	{
		START,
		STOP
	};

	static volatile State state = State::START;

	switch(e)
	{
	case Event::LB_START:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			switch(state)
			{
			case State::START:
				MOTOR.start();
				state = State::STOP;
				break;
			case State::STOP:
				MOTOR.right();
				MOTOR.fast();
				MOTOR.stop();
				state = State::START;
				break;
			}
		}
		break;
	case Event::LB_SWITCH:
		if(METAL_SENSOR.isMetal())
		{
			MOTOR.slow();
		}
		MOTOR.left();
		break;
	default:
		break;
	}
}

void HALTester::t_009(Event e)
{
	switch(e)
	{
	case Event::LB_HEIGHTSENSOR:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			MOTOR.start();
		}
		break;
	case Event::LB_SWITCH:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			SWITCH.open();
		}
		else
		{
			runIn(1000, [this](void) -> bool {
				SWITCH.close();

				return false;
			});
		}
		break;
	case Event::LB_END:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			MOTOR.stop();
		}
		break;
	default:
		break;
	}
}

void HALTester::t_010(Event e)
{
	switch(e)
	{
	case Event::BTN_ESTOP:
		if(BUTTONS.isPressed(Button::ESTOP))
		{
			LIGHTS.flash(Light::RED, 500);
		}
		else
		{
			LIGHTS.turnOff(Light::RED);
		}
		break;
	default:
		break;
	}
}

void HALTester::t_011(Event e)
{
	static timer_id_t timer = INVALID_TIMER_ID;
	auto& timer_r(timer); // To stop the compiler complaining; please disregard

	auto unregisterTimer = [&timer_r](void) {
		if(timer_r != INVALID_TIMER_ID)
		{
			lib::Timer::instance().unregisterCallback(timer_r);
			timer_r = INVALID_TIMER_ID;
		}
	};

	switch(e)
	{
	case Event::LB_START:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			MOTOR.start();
		}
		break;
	case Event::LB_RAMP:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
		{
			unregisterTimer();

			mTimers.push_back(timer = lib::Timer::instance().registerCallback([this](void) -> bool {
				MOTOR.stop();
				LEDS.turnOn(LED::RESET);
				LIGHTS.flash(Light::YELLOW, 1000);

				return false;
			}, 500));
		}
		else
		{
			unregisterTimer();
		}
		break;
	default:
		break;
	}
}

}}}
