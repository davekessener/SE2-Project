
#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_manager.h"
#include "system.h"

namespace esep { namespace base {

ConfigManager::ConfigManager(communication::IRecipient *handler, ConfigObject *config)
	: mHandler(handler)
	, mConfig(config)
	, HEIGHT_SENSOR(System::instance().get<hal::HeightSensor>())
	, SWITCH(System::instance().get<hal::Switch>())
	, LIGHT_BARRIERS(System::instance().get<hal::LightBarriers>())
	, LIGHTS(System::instance().get<hal::Lights>())
	, MOTOR(System::instance().get<hal::Motor>())
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
}

void ConfigManager::leave()
{
	SWITCH.close();
	MOTOR.right();
	MOTOR.fast();
	MOTOR.stop();
	LIGHTS.turnOff(Light::YELLOW);
}

void ConfigManager::accept(std::shared_ptr<communication::Packet> packet)
{
	switch (mState)
	{
	case State::STATE_0 :
		if(packet->message() == Message::RESUME)
			{
				LIGHTS.flash(Light::YELLOW, 500);
				MOTOR.right();
				MOTOR.start();

				mState = State::STATE_1;
			};
	break;
	case State::STATE_15 :
		if(packet->message() == Message::CONFIG_DONE)
		{
			MOTOR.stop();
			LIGHTS.turnOff(Light::YELLOW);
		};
	break;
	default :
		break;
	}
}

void ConfigManager::handle(hal::HAL::Event event)
{
	uint64_t actualTime;

	switch (mState)
	{
	case State::STATE_0 :
		if(event == Event::LB_START && LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			LIGHTS.flash(Light::YELLOW, 500);
			MOTOR.right();
			MOTOR.start();

			mState = State::STATE_1;
		};
	break;
	case State::STATE_1 :
		if(event == Event::LB_START && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			mTimestamp  = Timer::instance().elapsed();

			mState = State::STATE_2;
		};
	break;
	case State::STATE_2 :
		if(event == Event::LB_SWITCH && LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			SWITCH.open();

			mState = State::STATE_3;
		};
	break;
	case State::STATE_3 :
		if(event == Event::LB_END && LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime  = Timer::instance().elapsed();
			mStartToEndLong = (uint32_t) actualTime - mTimestamp;
			MOTOR.left();

			mState = State::STATE_4;
		};
	break;
	case State::STATE_4 :
		if(event == Event::LB_SWITCH && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			SWITCH.close();

			mState = State::STATE_5;
		};
	break;
	case State::STATE_5 :
		if(event == Event::LB_START && LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			MOTOR.right();
			mTimestamp = Timer::instance().elapsed();
			mTimestamp2 = Timer::instance().elapsed();

			mState = State::STATE_6;
		};
	break;
	case State::STATE_6 :
		if(event == Event::LB_HEIGHTSENSOR && LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			actualTime = Timer::instance().elapsed();
			mStartToHs = (uint32_t) actualTime - mTimestamp2;

			mState = State::STATE_7;
		};
	break;
	case State::STATE_7 :
		if(event == Event::LB_HEIGHTSENSOR && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_HEIGHTSENSOR))
		{
			mTimestamp2 = Timer::instance().elapsed();

			mState = State::STATE_8;
		};
	break;
	case State::STATE_8 :
		if(event == Event::LB_SWITCH && LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			actualTime = Timer::instance().elapsed();
			mHsToSwitch = (uint32_t) actualTime - mTimestamp2;
			SWITCH.open();

			mState = State::STATE_9;
		};
	break;
	case State::STATE_9 :
		if(event == Event::LB_SWITCH && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			mTimestamp2 = Timer::instance().elapsed();

			mState = State::STATE_10;
		};
	break;
	case State::STATE_10 :
		if(event == Event::LB_END && LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime = Timer::instance().elapsed();
			mSwitchToEnd = (uint32_t) actualTime - mTimestamp2;
			mStartToEnd = (uint32_t) actualTime - mTimestamp;
			MOTOR.left();

			mState = State::STATE_11;
		};
	break;
	case State::STATE_11 :
		if(event == Event::LB_SWITCH && !LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			SWITCH.close();

			mState = State::STATE_12;
		};
	break;
	case State::STATE_12 :
		if(event == Event::LB_START && LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			MOTOR.slow();
			MOTOR.right();
			mTimestamp = Timer::instance().elapsed();

			mState = State::STATE_13;
		};
	break;
	case State::STATE_13 :
		if(event == Event::LB_SWITCH && LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			SWITCH.open();
			//TODO measure HEIGHTSENSOR

			mState = State::STATE_14;
		};
	break;
	case State::STATE_14 :
		if(event == Event::LB_END && LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime = Timer::instance().elapsed();
			SWITCH.close();
			mStartToEndSlow = (uint32_t) actualTime - mTimestamp;
			mSlowFactor = mStartToEnd / (float) mStartToEndSlow;
			mTimeTolerance = 1 - ((mStartToEnd / (float) mStartToEndLong) + (mStartToEnd / (float) mStartToEndLong) * ConfigObject::TOLERANCE);

			// Save config and send message to handler
			auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::CONFIG_DONE);
			try {
				mConfig->setHeightSensor(1); // TODO measure HEIGHTSENSOR
				mConfig->setStartToHs(mStartToHs);
				mConfig->setHsToSwitch(mHsToSwitch);
				mConfig->setSwitchToEnd(mSwitchToEnd);
				mConfig->setSlowFactor(mSlowFactor);
				mConfig->setTimeTolerance(mTimeTolerance);
				mConfig->save();
			} catch (ConfigObject::InvalidDataException &e) {
				msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::CONFIG_FAILED);
			}

			mHandler->accept(msg);

			mState = State::STATE_15;
		};
	break;

	default:
		break;
	}
}

}}
