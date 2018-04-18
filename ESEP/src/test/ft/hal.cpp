#include <iostream>

#include "test/ft/hal.h"

#include "hal/physical.h"

namespace esep { namespace test { namespace functional {

HALTester::Cleaner::~Cleaner(void)
{
	f();

	hal.get<hal::Switch>().close();
	hal.get<hal::Motor>().right();
	hal.get<hal::Motor>().fast();
	hal.get<hal::Motor>().enable();
	hal.get<hal::Motor>().stop();
	hal.get<hal::LEDs>().turnOff(hal::LEDs::LED::START);
	hal.get<hal::LEDs>().turnOff(hal::LEDs::LED::RESET);
	hal.get<hal::LEDs>().turnOff(hal::LEDs::LED::Q1);
	hal.get<hal::LEDs>().turnOff(hal::LEDs::LED::Q2);
	hal.get<hal::Lights>().turnOff(hal::Lights::Light::RED);
	hal.get<hal::Lights>().turnOff(hal::Lights::Light::YELLOW);
	hal.get<hal::Lights>().turnOff(hal::Lights::Light::GREEN);
}

HALTester::HALTester(void)
	: mHAL(new hal::Physical)
	, mHALInterfaces(
		hal::Buttons(mHAL),
		hal::HeightSensor(mHAL),
		hal::LEDs(mHAL),
		hal::Switch(mHAL),
		hal::LightBarriers(mHAL),
		hal::Lights(mHAL),
		hal::MetalSensor(mHAL),
		hal::Motor(mHAL))
{
	mTests.push_back(&HALTester::t_011);

//	mTests.push_back(&HALTester::t_004);
//	mTests.push_back(&HALTester::t_005);
//	mTests.push_back(&HALTester::t_006);
//	mTests.push_back(&HALTester::t_007);
//	mTests.push_back(&HALTester::t_008);
//	mTests.push_back(&HALTester::t_009);
//	mTests.push_back(&HALTester::t_010);
//	mTests.push_back(&HALTester::t_011);
}

HALTester::~HALTester(void)
{
	delete mHAL;
}

void HALTester::run(void)
{
	auto no_handler = [](Event e) { };
	std::string line;

	std::cout << "Starting interactive HAL test.\n"
			  << "===========================================================================================\n";

	for(uint i = 0 ; i < mTests.size() ; ++i)
	{
		Cleaner cleanup([this](void) { clearTimers(); }, mHALInterfaces);
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

void HALTester::t_004(Event e)
{
	switch(e)
	{
	case Event::BTN_START:
		if(get<hal::Buttons>().isPressed(Button::START))
		{
			get<hal::LEDs>().turnOn(LED::START);
			get<hal::Lights>().turnOn(Light::GREEN);

			runIn(3000, [this](void) -> bool {
				get<hal::Lights>().turnOff(Light::GREEN);

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
		if(get<hal::Buttons>().isPressed(Button::STOP))
		{
			get<hal::Lights>().turnOn(Light::RED);

			runIn(3000, [this](void) -> bool {
				get<hal::Lights>().turnOff(Light::RED);

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
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_START))
		{
			get<hal::Motor>().start();
		}
		else
		{
			get<hal::Motor>().stop();
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
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_END))
		{
			get<hal::Motor>().left();
			get<hal::Motor>().start();
		}
		else
		{
			get<hal::Motor>().stop();
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
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_START))
		{
			switch(state)
			{
			case State::START:
				get<hal::Motor>().start();
				state = State::STOP;
				break;
			case State::STOP:
				get<hal::Motor>().right();
				get<hal::Motor>().fast();
				get<hal::Motor>().stop();
				state = State::START;
				break;
			}
		}
		break;
	case Event::LB_SWITCH:
		if(get<hal::MetalSensor>().isMetal())
		{
			get<hal::Motor>().slow();
		}
		get<hal::Motor>().left();
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
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			get<hal::Motor>().start();
		}
		break;
	case Event::LB_SWITCH:
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_SWITCH))
		{
			get<hal::Switch>().open();
		}
		else
		{
			get<hal::Switch>().close();
		}
		break;
	case Event::LB_END:
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_END))
		{
			get<hal::Motor>().stop();
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
		if(get<hal::Buttons>().isPressed(Button::ESTOP))
		{
			get<hal::Lights>().flash(Light::RED, 500);
		}
		else
		{
			get<hal::Lights>().turnOff(Light::RED);
		}
		break;
	default:
		break;
	}
}

void HALTester::t_011(Event e)
{
	static lib::Timer::Class::id_t timer = lib::Timer::Class::INVALID_TIMER_ID;
	auto& timer_r(timer);

	auto unregisterTimer = [&timer_r](void) {
		if(timer_r != lib::Timer::Class::INVALID_TIMER_ID)
		{
			lib::Timer::instance().unregisterCallback(timer_r);
		}
	};

	switch(e)
	{
	case Event::LB_START:
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_START))
		{
			get<hal::Motor>().start();
		}
		break;
	case Event::LB_RAMP:
		if(get<hal::LightBarriers>().isBroken(LightBarrier::LB_RAMP))
		{
			unregisterTimer();

			mTimers.push_back(timer = lib::Timer::instance().registerCallback([this](void) -> bool {
				get<hal::Motor>().stop();
				get<hal::LEDs>().turnOn(LED::RESET);
				get<hal::Lights>().flash(Light::YELLOW, 1000);

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

void HALTester::clearTimers(void)
{
	for(const auto& id : mTimers)
	{
		lib::Timer::instance().unregisterCallback(id);
	}

	mTimers.clear();
}

}}}
