#include <iostream>
#include <atomic>

#include "lib/arguments.h"

#include "hal/physical.h"

#include "hal/buttons.h"
#include "hal/height_sensor.h"
#include "hal/motor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"

#include "base/config_object.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace functional {

enum State {
	CONFIG_IDLE,
	CONFIG_RUNNING,
	IDLE,
	MEASURE_IDLE,
	MEASURE_RUNNING
};

void testHeightSensor(const lib::Arguments& args)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;
	typedef hal::LightBarriers::LightBarrier LightBarrier;
	typedef hal::Lights::Light Light;

	std::cout << "=== HEIGHTSENSOR TEST ======================================================================" << std::endl;

	hal::Physical hal;
	hal::Buttons btns(&hal);
	base::ConfigObject config;
	hal::HeightSensor height(&hal, &config);
	hal::Motor motor(&hal);
	hal::LightBarriers lightbarriers(&hal);
	hal::Lights lights(&hal);

	//Set config params
//	config.setHeightSensorMax(0b10001111);
//	config.setHeightSensorMin(0b11100010);
	config.setHsToSwitch(1);
	config.setSlowFactor(0.5);
	config.setStartToHs(1);
	config.setSwitchToEnd(1);
	config.setTimeTolerance(0.5);

//	if(config.isValid())
//	{
//		std::cout << "Valid ConfigObject!" << std::endl;
//	}
//	else
//	{
//
//		std::cout << "Invalid ConfigObject!" << std::endl;
//	}
//

	std::atomic<bool> running(true);
	State curState = State::IDLE;
	uint32_t measure_min;
	uint32_t samples_min;
	uint32_t measure_max;
	uint32_t samples_max;

//	hal.setCallback([&](Event e) {
//		switch(e)
//		{
//		case Event::BTN_STOP:
//			if(btns.isPressed(Button::STOP))
//			{
//				running = false;
//			}
//			break;
//
//		case Event::HEIGHT_SENSOR:
//			std::cout << lib::hex<16>(height.measure()) << " " << lib::bin<16>(height.measure()) << std::endl;
//			break;
//
//		default:
//			break;
//		}
//	});

		std::cout << "Press 'reset' for configuration or 'start' for measuring." << std::endl;
		hal.setCallback([&](Event e) {

			if (e == Event::BTN_STOP)
			{
				running = false;
				lights.turnOff(Light::GREEN);
				lights.turnOff(Light::YELLOW);
				motor.stop();
			}

			switch(curState)
			{

			case State::CONFIG_IDLE:
				switch (e)
				{
				case Event::LB_START:
					motor.right();
					motor.start();
					curState = State::CONFIG_RUNNING;
					lights.flash(Light::YELLOW, 1000);
					break;
				case Event::BTN_START:
					if (btns.isPressed(Button::START) && config.isValid())
					{
						curState = State::MEASURE_IDLE;
						std::cout << "Place an ITEM at the beginning of the system to start measuring." << std::endl;
						lights.turnOff(Light::YELLOW);
						lights.turnOn(Light::GREEN);
					}
					else if (btns.isPressed(Button::START))
					{
						std::cout << "You need to run the heightsensor configuration first!" << std::endl;
					}
					break;
				default:
					break;
				}
				break;

			case State::CONFIG_RUNNING:
				switch (e)
				{
				case Event::HEIGHT_SENSOR:
					measure_min += height.measure(false);
					samples_min++;
					break;
				case Event::LB_HEIGHTSENSOR:
					if (lightbarriers.isBroken(LightBarrier::LB_HEIGHTSENSOR))
					{
						samples_max = 3;
						uint32_t i;
						for (i = 0; i < samples_max; i++)
						{
							measure_max += height.measure(false);
							//lib::Timer::instance().sleep(10);
						}
					}
					else
					{
						measure_max /= samples_max;
						measure_min /= samples_min;
						config.setHeightSensorMin((uint16_t) 5); // TODO set sensor max
						config.setHeightSensorMax((uint16_t) 5); // TODO set sensor min
						curState = State::IDLE;
						lights.turnOff(Light::YELLOW);
						motor.stop();
					}
					break;
				default:
					break;
				}
				break;

			case State::MEASURE_IDLE:
				switch (e)
				{
				case Event::LB_START:
					motor.right();
					motor.start();
					curState = State::MEASURE_RUNNING;
					lights.flash(Light::GREEN, 1000);
					break;
				case Event::BTN_RESET:
					if (btns.isPressed(Button::RESET))
					{
						curState = State::CONFIG_IDLE;
						std::cout << "Place the highest ITEM at the beginning of the system to start configuration." << std::endl;
						lights.turnOff(Light::GREEN);
					}
					break;
				default:
					break;
				}
				break;

			case State::MEASURE_RUNNING:
				// TODO create profile
				if (e == Event::LB_HEIGHTSENSOR && !lightbarriers.isBroken(LightBarrier::LB_HEIGHTSENSOR))
				{
					curState = State::IDLE;
					std::cout << "Done measuring! You can now see the results in 'ft_profile.m'." << std::endl;
					lights.turnOff(Light::GREEN);
					motor.stop();
				}
				break;

			case State::IDLE:
				switch (e)
				{
				case Event::BTN_START:
					if (btns.isPressed(Button::START) && config.isValid())
					{
						curState = State::MEASURE_IDLE;
						std::cout << "Place an ITEM at the beginning of the system to start measuring." << std::endl;
						lights.turnOn(Light::GREEN);
					}
					else if (btns.isPressed(Button::START))
					{
						std::cout << "You need to run the heightsensor configuration first!" << std::endl;
					}
					break;
				case Event::BTN_RESET:
					if (btns.isPressed(Button::RESET))
					{
						curState = State::CONFIG_IDLE;
						std::cout << "Place the highest ITEM at the beginning of the system to start configuration." << std::endl;
						lights.turnOn(Light::YELLOW);
					}
					break;
				default:
					break;
				}
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
