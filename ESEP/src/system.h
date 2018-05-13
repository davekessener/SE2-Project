#ifndef ESEP_SYSTEM_H
#define ESEP_SYSTEM_H

#include "lib/utils.h"
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

			public:
				Impl( );
				~Impl( );
				void run(const lib::args_t&);
				template<typename T>
					T& get( )
						{ return mHALObjects.get<T>(); }
			private:
				hal::HAL *mHAL;
				hal_t mHALObjects;
		};
	}

	typedef lib::Singleton<system::Impl> System;
}

#endif
