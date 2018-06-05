
#include "test/ft/heightsensor/item_measurement.h"

#include <iostream>
#include <atomic>
#include <fstream>

#include "lib/arguments.h"

#include "hal/physical.h"

#include "hal/buttons.h"
#include "hal/height_sensor.h"
#include "hal/motor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"

#include "base/config_object.h"

#include "lib/timer.h"

namespace esep { namespace test { namespace functional { namespace h {

void ItemMeasurement::run(void)
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
	config.setMaxHandOverTime(3000);
	config.setHsToSwitch(1);
	config.setStartToHs(1);
	config.setSwitchToEnd(1);
	config.setTimeTolerance(0.5);

	std::atomic<bool> running(true);
	State curState = State::IDLE;
	uint32_t measure_min = 0;
	uint32_t samples_min = 0;
	uint32_t measure_max = 0;
	uint32_t samples_max = 0;

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
			case Event::BTN_RESET:
				if (btns.isPressed(Button::RESET))
				{
					motor.right();
					motor.start();
					curState = State::CONFIG_MIN;
					lights.flash(Light::YELLOW, 1000);
				}
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

		case State::CONFIG_MIN:
			switch (e)
			{
			case Event::HEIGHT_SENSOR:
				measure_min += height.measure(false);
				samples_min++;
				break;
			case Event::LB_START:
				if (!lightbarriers.isBroken(LightBarrier::LB_START))
				{
					curState = State::CONFIG_RUNNING;
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
				if (lightbarriers.isBroken(LightBarrier::LB_HEIGHTSENSOR))
				{
					measure_max += height.measure(false);
					samples_max++;
				}
				break;
			case Event::LB_HEIGHTSENSOR:
				if (!lightbarriers.isBroken(LightBarrier::LB_HEIGHTSENSOR))
				{
					uint16_t max = (uint16_t) (measure_max / samples_max);
					uint16_t min = (uint16_t) (measure_min / samples_min);
					config.setHeightSensorMin(min);
					config.setHeightSensorMax(max);
					config.save();
					std::cout << "Done measuring!." << std::endl;
					std::cout << "MAX = 0x" << lib::hex<16>(max) << " (" << samples_max << " Samples)" << std::endl;
					std::cout << "MIN = 0x" << lib::hex<16>(min) << " (" << samples_min << " Samples)" << std::endl;
					samples_min = 0;
					samples_max = 0;
					measure_min = 0;
					measure_max = 0;
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
			if (e == Event::LB_HEIGHTSENSOR && !lightbarriers.isBroken(LightBarrier::LB_HEIGHTSENSOR))
			{
				curState = State::IDLE;
				lights.turnOff(Light::GREEN);
				motor.stop();
				std::cout << "Done measuring! You can now see the results in 'ft_profile.txt'." << std::endl;
				save();
			}
			else if (e == Event::HEIGHT_SENSOR)
			{
				// create profile
				measure(height.measure());
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


void ItemMeasurement::measure(uint16_t val)
{
	uint32_t curTime = (uint32_t) lib::Timer::instance().elapsed();
	if(mProfile.size() == 0)
	{
		mStartTime = curTime;
	}

	if (val > 0) mProfile.push_back(std::make_pair(curTime-mStartTime, val));
}

void ItemMeasurement::save(void)
{
	const std::string path = "ft_profile.txt";
	std::ofstream profileFile(path);

	if(profileFile.is_open())
	{
		for(auto const& pair: mProfile)
		{
			profileFile << pair.first << " " << pair.second << "\n";
		}
	}
	else
	{
		MXT_THROW_EX(CouldNotOpenFileException);
	}

	clearProfile();
}

void ItemMeasurement::clearProfile(void)
{
	mProfile.clear();
	mStartTime = 0;
}

}}}}

