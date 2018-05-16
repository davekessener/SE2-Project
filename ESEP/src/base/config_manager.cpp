#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_manager.h"
#include "system.h"

#include "hal/height_sensor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/motor.h"
#include "hal/switch.h"

namespace esep { namespace base {

typedef hal::LightBarriers::LightBarrier LightBarrier;
typedef hal::Lights::Light Light;
typedef hal::HAL::Event Event;

ConfigManager::ConfigManager(communication::IRecipient *handler, ConfigObject *config)
	: mHandler(handler)
	, mConfig(config)
	, mState(State::STATE_0)
	, mTimestamp(0)
	, mTimestamp2(0)
	, mHeightSensor(0)
	, mStartToEnd(0)
	, mStartToEndLong(0)
	, mStartToEndSlow(0)
	, mStartToHs(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
	, mSlowFactor(0)
	, mTimeTolerance(0)
{

}

void ConfigManager::enter()
{
	mState = State::STATE_0;
	HAL_LIGHTS.flash(Light::GREEN, T_PERIOD);
}

void ConfigManager::leave()
{
	HAL_SWITCH.close();
	HAL_MOTOR.right();
	HAL_MOTOR.fast();
	HAL_MOTOR.stop();
	HAL_LIGHTS.turnOff(Light::YELLOW);
	HAL_LIGHTS.turnOff(Light::GREEN);
}

void ConfigManager::accept(std::shared_ptr<communication::Packet> packet)
{
	if(packet->message() == Message::Run::RESUME)
	{
		HAL_LIGHTS.turnOn(Light::YELLOW);
		HAL_MOTOR.right();
		HAL_MOTOR.start();
	}
	else if(packet->message() == Message::Run::SUSPEND)
	{
		HAL_LIGHTS.turnOff(Light::YELLOW);
		HAL_MOTOR.stop();
	}
}

void ConfigManager::handle(hal::HAL::Event event)
{
	uint64_t actualTime = 0;

#define ELAPSED lib::Timer::instance().elapsed()
	switch (mState)
	{
	case State::STATE_0 :
		if(event == Event::LB_START && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			HAL_LIGHTS.turnOn(Light::YELLOW);
			HAL_MOTOR.right();
			HAL_MOTOR.start();

			mHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Config::START));

			mState = State::STATE_1;
		};
	break;

	case State::STATE_1 :
		if(event == Event::LB_START && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			mTimestamp  = ELAPSED;

			mState = State::STATE_2;
		};
	break;

	case State::STATE_2 :
		if(event == Event::LB_SWITCH && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			HAL_SWITCH.open();

			mState = State::STATE_3;
		};
	break;

	case State::STATE_3 :
		if(event == Event::LB_END && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime  = ELAPSED;
			mStartToEndLong = (uint32_t) actualTime - mTimestamp;
			HAL_MOTOR.left();

			mState = State::STATE_4;
		};
	break;

	case State::STATE_4 :
		if(event == Event::LB_SWITCH && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			HAL_SWITCH.close();

			mState = State::STATE_5;
		};
	break;

	case State::STATE_5 :
		if(event == Event::LB_START && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			HAL_MOTOR.right();
			mTimestamp = mTimestamp2 = ELAPSED;

			mState = State::STATE_6;
		};
	break;

	case State::STATE_6 :
		if(event == Event::LB_HEIGHTSENSOR && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			actualTime = ELAPSED;
			mStartToHs = (uint32_t) actualTime - mTimestamp2;

			mState = State::STATE_7;
		};
	break;

	case State::STATE_7 :
		if(event == Event::LB_HEIGHTSENSOR && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			mTimestamp2 = ELAPSED;

			mState = State::STATE_8;
		};
	break;

	case State::STATE_8 :
		if(event == Event::LB_SWITCH && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			actualTime = ELAPSED;
			mHsToSwitch = (uint32_t) actualTime - mTimestamp2;
			HAL_SWITCH.open();

			mState = State::STATE_9;
		};
	break;

	case State::STATE_9 :
		if(event == Event::LB_SWITCH && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			mTimestamp2 = ELAPSED;

			mState = State::STATE_10;
		};
	break;

	case State::STATE_10 :
		if(event == Event::LB_END && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime = ELAPSED;
			mSwitchToEnd = (uint32_t) actualTime - mTimestamp2;
			mStartToEnd = (uint32_t) actualTime - mTimestamp;
			HAL_MOTOR.left();

			mState = State::STATE_11;
		};
	break;

	case State::STATE_11 :
		if(event == Event::LB_SWITCH && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			HAL_SWITCH.close();

			mState = State::STATE_12;
		};
	break;

	case State::STATE_12 :
		if(event == Event::LB_START && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			HAL_MOTOR.slow();
			HAL_MOTOR.right();
			mTimestamp = ELAPSED;

			mState = State::STATE_13;
		};
	break;

	case State::STATE_13 :
		if(event == Event::LB_SWITCH && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			HAL_SWITCH.open();
			//TODO measure HEIGHTSENSOR

			mState = State::STATE_14;
		};
	break;

	case State::STATE_14 :
		if(event == Event::LB_END && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime = ELAPSED;
			HAL_SWITCH.close();
			HAL_MOTOR.stop();
			mStartToEndSlow = (uint32_t) actualTime - mTimestamp;
			mSlowFactor = mStartToEnd / (float) mStartToEndSlow;
			mTimeTolerance = 1 - ((mStartToEnd / (float) mStartToEndLong) + (mStartToEnd / (float) mStartToEndLong) * ConfigObject::TOLERANCE);

			// Save config and send message to handler
			auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::Config::FAILED);

			try
			{
				mConfig->setHeightSensor(1); // TODO measure HEIGHTSENSOR
				mConfig->setStartToHs(mStartToHs);
				mConfig->setHsToSwitch(mHsToSwitch);
				mConfig->setSwitchToEnd(mSwitchToEnd);
				mConfig->setSlowFactor(mSlowFactor);
				mConfig->setTimeTolerance(mTimeTolerance);
				mConfig->save();

				msg->message(Message::Config::DONE);
			}
			catch(ConfigObject::InvalidDataException const& e)
			{
			}

			mHandler->accept(msg);

			mState = State::STATE_15;
		};
		break;

	case State::STATE_15:
		break;
	}
#undef ELAPSED
}

}}
