#include <iostream>
#include <atomic>

#include "lib/arguments.h"

#include "hal/physical.h"

#include "hal/buttons.h"
#include "hal/height_sensor.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace functional {

void testHeightSensor(const lib::Arguments& args)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	std::cout << "=== HEIGHTSENSOR TEST ======================================================================" << std::endl;

	hal::Physical hal;
	hal::Buttons btns(&hal);
	hal::HeightSensor height(&hal);

	std::atomic<bool> running(true);

	hal.setCallback([&](Event e) {
		switch(e)
		{
		case Event::BTN_STOP:
			if(btns.isPressed(Button::STOP))
			{
				running = false;
			}
			break;

		case Event::HEIGHT_SENSOR:
			std::cout << lib::bin<12>(height.measure()) << std::endl;
			break;

		default:
			break;
		}
	});

	while(running.load())
	{
		lib::Timer::instance().sleep(10);
	}

	hal.setCallback([](Event e) {});

	std::cout << "============================================================================================" << std::endl;
}

}}}
