#include <iostream>

#include "test/ft/hal/tester.h"

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
	mRunning = false;
	mTests.push_back(std::make_pair("T-004", &HALTester::t_004));
	mTests.push_back(std::make_pair("T-005", &HALTester::t_005));
	mTests.push_back(std::make_pair("T-006", &HALTester::t_006));
	mTests.push_back(std::make_pair("T-007", &HALTester::t_007));
	mTests.push_back(std::make_pair("T-008", &HALTester::t_008));
	mTests.push_back(std::make_pair("T-009", &HALTester::t_009));
	mTests.push_back(std::make_pair("T-010", &HALTester::t_010));
	mTests.push_back(std::make_pair("T-011", &HALTester::t_011));
}

HALTester::~HALTester(void)
{
	delete mHAL;
}

void HALTester::run(void)
{
	std::string line;

	resetHAL();

	std::cout << "# === HAL TEST ===============================================================================\n";

	mRunning = true;

	while(mRunning)
	{
		std::cout << "There are " << mTests.size() << " tests loaded.\n"
				  << "ID | Name\n---+-------\n";

		for(uint i = 0 ; i < mTests.size() ; ++i)
		{
			std::cout << std::setw(2) << (i + 1) << " | " << mTests[i].first << "\n";
		}

		std::cout << "[Enter 'q' to quit]\n"
				  << "Please enter test ID (or * for all): ";

		std::getline(std::cin, line);

		if(line.empty()) continue;
		else if(line[0] >= '0' && line[0] <= '9')
		{
			runTest(line[0] - '0' - 1);
		}
		else switch(line[0])
		{
		case 'q':
			mRunning = false;
			break;
		case '*':
			for(uint i = 0 ; i < mTests.size() ; ++i)
			{
				runTest(i);
			}
			break;
		}
	}

	std::cout << "# ============================================================================================" << std::endl;
}

void HALTester::runTest(uint i)
{
	std::string line;
	lib::Deferred cleanup([this](void) { resetHAL(); clearTimers(); });
	auto no_handler = [](Event e) { };
	test_fn f = mTests[i].second;

	std::cout << "Running test " << mTests[i].first << "\n";

	mHAL->setCallback([this, f](Event e) { (this->*f)(e); });

	std::cout << "[running] ... (press ENTER to stop)";

	std::getline(std::cin, line);

	mHAL->setCallback(no_handler);

	std::cout << "\n--- END OF TEST ---------------------------------------------------------------------------\n\n";
}

// # -----------------------------------------------------------------------------

void HALTester::clearTimers(void)
{
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
			runIn(1000, [this](void) {
				SWITCH.close();
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
	switch(e)
	{
	case Event::LB_START:
		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			MOTOR.start();
		}
		break;

	case Event::LB_RAMP:
		clearTimers();

		if(LIGHT_BARRIERS.isBroken(LightBarrier::LB_RAMP))
		{
			runIn(500, [this](void) {
				MOTOR.stop();
				LEDS.turnOn(LED::RESET);
				LIGHTS.flash(Light::YELLOW, 1000);
			});
		}
		break;

	case Event::BTN_RESET:
		if(BUTTONS.isPressed(Button::RESET))
		{
			LIGHTS.turnOff(Light::YELLOW);
			LEDS.turnOff(LED::RESET);
		}
		break;

	default:
		break;
	}
}

}}}
