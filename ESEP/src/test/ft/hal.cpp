#include <iostream>

#include "test/ft/hal.h"

#include "hal/physical.h"

namespace esep { namespace test { namespace functional {

HALTester::Cleaner::~Cleaner(void)
{
	hal.get<hal::Switch>().close();
	hal.get<hal::Motor>().right();
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
	mTests.push_back(&HALTester::t_004);
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
	for(const auto& id : mTimers)
	{
		lib::Timer::instance().unregisterTimer(id);
	}

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
		Cleaner cleanup(mHALInterfaces);
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

			lib::Timer::instance().registerCallback([this](void) -> bool {
				get<hal::Lights>().turnOff(Light::GREEN);

				return false;
			}, 3000);
		}
		break;
	default:
		break;
	}
}

void HALTester::t_005(Event e)
{
}

void HALTester::t_006(Event e)
{
}

void HALTester::t_007(Event e)
{
}

void HALTester::t_008(Event e)
{
}

void HALTester::t_009(Event e)
{
}

void HALTester::t_010(Event e)
{
}

void HALTester::t_011(Event e)
{
}

}}}
