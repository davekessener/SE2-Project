#ifndef ESEP_SYSTEM_H
#define ESEP_SYSTEM_H

#include "lib/utils.h"
#include "lib/arguments.h"
#include "lib/singleton.h"
#include "lib/tuple.h"

#include "hal/hal.h"
#include "hal/buffer.h"
#include "hal/buttons.h"
#include "hal/height_sensor.h"
#include "hal/leds.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/metal_sensor.h"
#include "hal/motor.h"
#include "hal/switch.h"


#define HAL_BUTTONS			::esep::System::instance().get<::esep::hal::Buttons>()
#define HAL_HEIGHT_SENSOR	::esep::System::instance().get<::esep::hal::HeightSensor>()
#define HAL_LEDS			::esep::System::instance().get<::esep::hal::LEDs>()
#define HAL_LIGHT_BARRIERS	::esep::System::instance().get<::esep::hal::LightBarriers>()
#define HAL_LIGHTS			::esep::System::instance().get<::esep::hal::Lights>()
#define HAL_METAL_SENSOR	::esep::System::instance().get<::esep::hal::MetalSensor>()
#define HAL_MOTOR			::esep::System::instance().get<::esep::hal::Motor>()
#define HAL_SWITCH			::esep::System::instance().get<::esep::hal::Switch>()

namespace esep
{
	namespace system
	{
		class Impl
		{
			typedef lib::MakeTuple<
				hal::Buttons,
				hal::HeightSensor,
				hal::LEDs,
				hal::Switch,
				hal::LightBarriers,
				hal::Lights,
				hal::MetalSensor,
				hal::Motor
			> hal_t;

			MXT_DEFINE_E(InvalidRunArgumentException);
			MXT_DEFINE_E(ConnectionException);

			public:
				Impl( );
				~Impl( );
				void run(const lib::Arguments&);
				template<typename T>
					T& get( )
						{ return mHALObjects.get<T>(); }

			private:
				hal::HAL *mHAL;
				hal_t mHALObjects;
				base::ConfigObject mConfig;
		};
	}

	typedef lib::Singleton<system::Impl> System;
}

#endif
