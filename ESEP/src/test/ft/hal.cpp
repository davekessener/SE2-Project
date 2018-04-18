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
	for(uint i = 0 ; i < mTests.size() ; ++i)
	{
		Cleaner cleanup(mHALInterfaces);
		test_fn f = mTests[i];

		std::cout << "Running test T-" << std::setw(3) << std::setfill('0') << (i + 4) << "\n"
				  << "Press ENTER to begin ...";

		std::cin.ignore();
		std::cin.get();

		mHAL->setCallback([this, f](Event e) { (this->*f)(e); });

		std::cout << "[running] ... (press ENTER to stop)";

		std::cin.ignore();
		std::cin.get();

		std::cout << "\n=== END OF TEST ===========================================================================\n";
	}
}

// # -----------------------------------------------------------------------------

void HALTester::t_004(Event e)
{
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
