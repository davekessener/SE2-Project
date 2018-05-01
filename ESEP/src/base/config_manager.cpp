
#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_manager.h"
#include "system.h"

#define MXT_TIME_NOW std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())

namespace esep { namespace base {

ConfigManager::ConfigManager(communication::IRecipient *baseHandler, ConfigObject *config)
	: mBaseHandler(baseHandler)
	, mConfig(config)
	, mRunning(false)
	, HEIGHT_SENSOR(System::instance().get<hal::HeightSensor>())
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHT_BARRIERS(System::instance().get<hal::LightBarriers>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, MOTOR(System::instance().get<hal::Motor>())
	, mStep(0)
	, mTimestamp(0)
	, mHeightSensor(0)
	, mStartToHs(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
	, mSlowFactor(0)
	, mBackwardFactor(0)
{

}

void ConfigManager::enter()
{
	mRunning= true;
}

void ConfigManager::leave()
{
	mStep = 0;
}

void ConfigManager::handle(hal::HAL::Event event)
{
	if (mRunning)
	{
		run(event);
	}
}

void ConfigManager::accept(std::shared_ptr<communication::Packet> packet)
{
	// clear (for now)
}

void ConfigManager::run(hal::HAL::Event event)
{
	// here main implementation
}

}}
