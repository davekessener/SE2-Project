#ifndef ESEP_HAL_H
#define ESEP_HAL_H

#include "lib/singleton.h"
#include "lib/tuple.h"
#include "lib/console.h"

#include "base/config_object.h"

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

#define HAL_BUTTONS			::esep::HAL::instance().get<::esep::hal::Buttons>()
#define HAL_HEIGHT_SENSOR	::esep::HAL::instance().get<::esep::hal::HeightSensor>()
#define HAL_LEDS			::esep::HAL::instance().get<::esep::hal::LEDs>()
#define HAL_LIGHT_BARRIERS	::esep::HAL::instance().get<::esep::hal::LightBarriers>()
#define HAL_LIGHTS			::esep::HAL::instance().get<::esep::hal::Lights>()
#define HAL_METAL_SENSOR	::esep::HAL::instance().get<::esep::hal::MetalSensor>()
#define HAL_MOTOR			::esep::HAL::instance().get<::esep::hal::Motor>()
#define HAL_SWITCH			::esep::HAL::instance().get<::esep::hal::Switch>()
#define HAL_CONSOLE			::esep::lib::Console::instance()

namespace esep
{
	namespace hal
	{
		class Holder
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

			public:
			MXT_DEFINE_E(HALNotInstantiatedException);

			public:
				void instantiate(HAL *, base::ConfigObject *);
				void clear( );

				template<typename T>
				T& get( )
				{
					if(!mHAL)
					{
						MXT_THROW_EX(HALNotInstantiatedException);
					}

					return mHALObjects->get<T>();
				}

			private:
				HAL *mHAL;
				std::unique_ptr<hal_t> mHALObjects;

			private:
				Holder( );
				~Holder( ) { }
				Holder(const Holder&) = delete;
				Holder& operator=(const Holder&) = delete;

				friend class tml::policy::DynamicCreation<Holder>;
		};
	}

	typedef lib::Singleton<hal::Holder, 1> HAL;
}

#endif
