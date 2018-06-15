#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_manager.h"
#include "hal.h"
#include "lib/utils.h"

#include "hal/height_sensor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/motor.h"
#include "hal/switch.h"
#include "hal/height_sensor.h"

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
	, mHeightSensorMin(0)
	, mHeightSensorMax(0)
	, mItemInLB(0)
	, mStartToEnd(0)
	, mStartToEndLong(0)
	, mStartToEndSlow(0)
	, mStartToHs(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
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
			mTimer = lib::Timer::instance().registerAsync([this](void) {
				HAL_MOTOR.right();
				HAL_MOTOR.start();
			}, T_PERIOD);

			//HAL_MOTOR.start();
			mHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Config::START));

			mState = State::STATE_1;
		};
	break;

	case State::STATE_1 :
		if(event == Event::LB_START && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_START))
		{
			mTimestamp  = ELAPSED;

			//measure HEIGHTSENSOR MINIMUM [1]
			MXT_LOG_INFO("HeightSensor measured ", lib::hex<16>(HAL_HEIGHT_SENSOR.measure()));
			mHeightSensorMin = HAL_HEIGHT_SENSOR.measure(false);

			mState = State::STATE_2;
		};
	break;

	case State::STATE_2 :
		if(event == Event::LB_SWITCH && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			HAL_SWITCH.open();

			//measure HEIGHTSENSOR MINIMUM [2]
			MXT_LOG_INFO("HeightSensor measured ", lib::hex<16>(HAL_HEIGHT_SENSOR.measure()));

			mState = State::STATE_3;
		};
	break;

	case State::STATE_3 :
		if(event == Event::LB_END && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime  = ELAPSED;
			mStartToEndLong = (uint32_t) actualTime - mTimestamp;
			HAL_MOTOR.left();

			//measure HEIGHTSENSOR MINIMUM [3]
			MXT_LOG_INFO("HeightSensor measured ", lib::hex<16>(HAL_HEIGHT_SENSOR.measure()));
			mHeightSensorMin = HAL_HEIGHT_SENSOR.measure(false);

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

			//measure HEIGHTSENSOR MAXIMUM
			MXT_LOG_INFO("HeightSensor measured ", lib::hex<16>(HAL_HEIGHT_SENSOR.measure()));
			mHeightSensorMax = HAL_HEIGHT_SENSOR.measure(false);

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

			mTimestamp2 = ELAPSED;

			HAL_SWITCH.open();

			mState = State::STATE_9;
		};
	break;

	case State::STATE_9 :
		if(event == Event::LB_SWITCH && !HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_SWITCH))
		{
			actualTime = ELAPSED;
			mItemInLB = (uint32_t) actualTime - mTimestamp2;

			mTimestamp2 = ELAPSED;

			mState = State::STATE_10;
		};
	break;

	case State::STATE_10 :
		if(event == Event::LB_END && HAL_LIGHT_BARRIERS.isBroken(LightBarrier::LB_END))
		{
			actualTime = ELAPSED;
			HAL_SWITCH.close();
			HAL_MOTOR.stop();
			mSwitchToEnd = (uint32_t) actualTime - mTimestamp2;
			mStartToEnd = (uint32_t) actualTime - mTimestamp;
			mTimeTolerance = 1 - (mStartToEnd / (float) mStartToEndLong);

			if(mTimeTolerance < 0.0) mTimeTolerance = 0;

			// Save config and send message to handler
			auto msg = std::make_shared<communication::Packet>(Location::BASE, Location::MASTER, Message::Config::FAILED);

			try
			{
				MXT_LOG_INFO("HS min is ", mHeightSensorMin);
				MXT_LOG_INFO("HS max is ", mHeightSensorMax);
				MXT_LOG_INFO("Start to HS is ", mStartToHs);
				MXT_LOG_INFO("HS to switch is ", mHsToSwitch);
				MXT_LOG_INFO("switch to end is ", mSwitchToEnd);
				MXT_LOG_INFO("TimeTolerance is ", mTimeTolerance);
				MXT_LOG_INFO("ItemInLB is ", mItemInLB);

				mConfig->setHeightSensorMin(mHeightSensorMin);
				mConfig->setHeightSensorMax(mHeightSensorMax);
				mConfig->setItemInLB(mItemInLB);
				mConfig->setStartToHs(mStartToHs);
				mConfig->setHsToSwitch(mHsToSwitch);
				mConfig->setSwitchToEnd(mSwitchToEnd);
				mConfig->setTimeTolerance(mTimeTolerance);
				mConfig->save();

				msg->message(Message::Config::DONE);
			}
			catch(ConfigObject::InvalidDataException const& e)
			{
				MXT_LOG_WARN("Failed configuration: ", e.what());
			}

			mHandler->accept(msg);

			mState = State::STATE_11;
		};
		break;

	case State::STATE_11:
		break;
	}
#undef ELAPSED
}

}}
