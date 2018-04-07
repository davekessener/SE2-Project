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

namespace esep
{
	namespace system
	{
		class Impl
		{
			typedef lib::Tuple<tml::MakeTypeList<
				hal::Buttons,
				hal::HeightSensor,
				hal::LEDs,
				hal::LightBarriers,
				hal::Lights,
				hal::MetalSensor,
				hal::Motor
			>> hal_t;

			public:
				Impl( );
				~Impl( );
				void run( );
				uint64_t elapsed( );
				void sleep(uint);
				void updateHAL(hal::HAL::Field f, uint32_t v) { mHAL->update(f, v); }
				template<typename T>
					T& get( )
						{ return mHALObjects.get<T>(); }
			private:
				hal::Buffer *mHAL;
				hal_t mHALObjects;
				std::chrono::time_point<std::chrono::system_clock> mSystemStart;
		};
	}

	typedef lib::SingletonHolder<system::Impl> System;
}

#endif
