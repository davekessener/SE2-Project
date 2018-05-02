
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
	, mStartToHsBackward(0)
	, mStartToHsSlow(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
	, mSlowFactor(0)
{

}

void ConfigManager::enter()
{
	mRunning = true;
}

void ConfigManager::leave()
{
	mRunning = false;
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
	typedef hal::HAL::Event Event;
	typedef hal::LightBarriers::LightBarrier LightBarrier;
	typedef communication::Packet::Location Location;
	typedef communication::Packet::Message Message;

	std::chrono::milliseconds msTime;
	uint32_t actualTime;

	switch (mStep)
	{
	case 0 :
		if(event == Event::LB_START && LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			MOTOR.right();
			MOTOR.start();
			//TODO measure HeightSensor
		};
	break;
	case 1 :
		if(event == hal::HAL::Event::LB_HEIGHTSENSOR && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_HEIGHTSENSOR))
		{
			MOTOR.left();
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			mTimestamp  = msTime.count();
		};
	break;
	case 2 :
		if(event == hal::HAL::Event::LB_START && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_START))
		{
			MOTOR.slow();
			MOTOR.right();
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			actualTime  = msTime.count();
			mStartToHsBackward = actualTime - mTimestamp;
			mTimestamp = msTime.count();
		};
	break;
	case 3 :
		if(event == hal::HAL::Event::LB_HEIGHTSENSOR && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_HEIGHTSENSOR))
		{
			MOTOR.left();
			MOTOR.fast();
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			actualTime  = msTime.count();
			mStartToHsSlow = actualTime - mTimestamp;
		};
	break;
	case 4 :
		if(event == hal::HAL::Event::LB_START && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_START))
		{
			MOTOR.right();
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			mTimestamp  = msTime.count();
		};
	break;
	case 5 :
		if(event == hal::HAL::Event::LB_HEIGHTSENSOR && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_HEIGHTSENSOR))
		{
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			actualTime  = msTime.count();
			mStartToHs = actualTime - mTimestamp;
		};
	break;
	case 6 :
		if(event == hal::HAL::Event::LB_HEIGHTSENSOR && !LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_HEIGHTSENSOR))
		{
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			mTimestamp  = msTime.count();
		};
	break;
	case 7 :
		if(event == hal::HAL::Event::LB_SWITCH && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_SWITCH))
		{
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			actualTime  = msTime.count();
			SWITCH.open();  // CRITICAL: SWITCH COULD BE OPEN FOREVER
			mHsToSwitch = actualTime - mTimestamp;
		};
	break;
	case 8 :
		if(event == hal::HAL::Event::LB_SWITCH && !LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_SWITCH))
		{
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			mTimestamp  = msTime.count();
		};
	break;
	case 9 :
		if(event == hal::HAL::Event::LB_END && LIGHT_BARRIERS.isBroken(hal::LightBarriers::LightBarrier::LB_END))
		{
			msTime = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now().time_since_epoch());
			actualTime  = msTime.count();
			MOTOR.stop();
			SWITCH.close();
			mSwitchToEnd = actualTime - mTimestamp;
			mSlowFactor = mStartToHsSlow / (float) mStartToHs;

			// Save to object
			mConfig->setHeightSensor(mHeightSensor);
			mConfig->setStartToHs(mStartToHs);
			mConfig->setHsToSwitch(mHsToSwitch);
			mConfig->setSwitchToEnd(mSwitchToEnd);
			mConfig->setSlowFactor(mSlowFactor);
			mConfig->save();

			// send message
			auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::CONFIG_DONE);
			mBaseHandler->accept(msg);
		};
	break;

	default:
		break;
	}
}

}}
