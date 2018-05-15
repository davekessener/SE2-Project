#ifndef SRC_BASE_CONFIG_MANAGER_H_
#define SRC_BASE_CONFIG_MANAGER_H_

#include <memory>
#include <chrono>

#include "base/IManager.h"
#include "base/config_object.h"
#include "hal/hal.h"
#include "hal/height_sensor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/motor.h"
#include "hal/switch.h"

namespace esep
{
	namespace base
	{
		class ConfigManager : public IManager
		{
			typedef hal::LightBarriers::LightBarrier LightBarrier;
			typedef hal::Lights::Light Light;
			typedef hal::HAL::Event Event;
			typedef communication::Packet::Location Location;
			typedef communication::Packet::Message Message;
			typedef lib::Timer Timer;

			enum class State
			{
				STATE_0, STATE_1, STATE_2, STATE_3,
				STATE_4, STATE_5, STATE_6, STATE_7,
				STATE_8, STATE_9, STATE_10, STATE_11,
				STATE_12, STATE_13, STATE_14, STATE_15,
			};

			public:
				ConfigManager(communication::IRecipient *, ConfigObject *);
				void enter() override;
				void leave() override;
				void handle(hal::HAL::Event) override;
				void accept(std::shared_ptr<communication::Packet>) override;

			private:
				communication::IRecipient * const mHandler;
				ConfigObject * const mConfig;
				hal::HeightSensor& HEIGHT_SENSOR;
				hal::Switch& SWITCH;
				hal::LightBarriers& LIGHT_BARRIERS;
				hal::Lights& LIGHTS;
				hal::Motor& MOTOR;
				State mState;
				uint64_t mTimestamp;
				uint64_t mTimestamp2;
				uint16_t mHeightSensor;
				uint32_t mStartToEnd;
				uint32_t mStartToEndLong;
				uint32_t mStartToEndSlow;
				uint32_t mStartToHs;
				uint32_t mHsToSwitch;
				uint32_t mSwitchToEnd;
				float mSlowFactor;
				float mTimeTolerance;
		};
}
}

#endif
