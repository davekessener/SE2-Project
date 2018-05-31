#include <iostream>
#include <atomic>

#include "lib/arguments.h"

#include "hal/physical.h"

#include "hal/buttons.h"
#include "hal/height_sensor.h"

#include "base/config_object.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace functional {

void testHeightSensor(const lib::Arguments& args)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	std::cout << "=== HEIGHTSENSOR TEST ======================================================================" << std::endl;

	hal::Physical hal;
	hal::Buttons btns(&hal);
	base::ConfigObject config;
	hal::HeightSensor height(&hal, &config);


	//Set config params
	config.setHeightSensorMax(0b10001111);
	config.setHeightSensorMin(0b11100010);
	config.setHsToSwitch(1);
	config.setSlowFactor(0.5);
	config.setStartToHs(1);
	config.setSwitchToEnd(1);
	config.setTimeTolerance(0.5);

	if(config.isValid())
	{
		std::cout << "Valid ConfigObject!" << std::endl;
	}
	else
	{

		std::cout << "Invalid ConfigObject!" << std::endl;
	}


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
			std::cout << lib::hex<16>(height.measure()) << " " << lib::bin<16>(height.measure()) << std::endl;
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
