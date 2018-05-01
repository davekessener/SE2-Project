
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
			public:
				ConfigManager(communication::IRecipient *, ConfigObject *);
				~ConfigManager() { };
				void enter();
				void leave();
				void handle(hal::HAL::Event);
				void accept(std::shared_ptr<communication::Packet>);

			private:
				void run(hal::HAL::Event);

			private:
				communication::IRecipient *mBaseHandler;
				ConfigObject *mConfig;
				bool mRunning;
				hal::HeightSensor& HEIGHT_SENSOR;
				hal::Switch& SWITCH;
				hal::LightBarriers& LIGHT_BARRIERS;
				hal::Lights& LIGHTS;
				hal::Motor& MOTOR;
				uint32_t mStep;
				time_t mTimestamp;
				uint16_t mHeightSensor;
				uint32_t mStartToHs;
				uint32_t mHsToSwitch;
				uint32_t mSwitchToEnd;
				float mSlowFactor;
				float mBackwardFactor;
		};
}
}

#endif
