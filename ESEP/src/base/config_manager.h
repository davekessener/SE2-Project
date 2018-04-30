
#ifndef SRC_BASE_CONFIG_MANAGER_H_
#define SRC_BASE_CONFIG_MANAGER_H_

#include "communication/IRecipient.h"
#include "base/config_object.h"

namespace esep
{
	namespace base
	{
		class ConfigManager : public IManager
		{
			public:
				ConfigManager(communication::IRecipient *, ConfigObject *);
				~ConfigManager();

			private:
				void run();

			private:
				ConfigObject *mConfig;
				bool mRunning;
				hal::HeightSensor HEIGHT_SENSOR;
				hal::Switch SWITCH;
				hal::LightBarriers LIGHT_BARRIERS;
				hal::Lights LIGHTS;
				hal::Motor MOTOR;
		};
}
}

#endif
