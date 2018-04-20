#ifndef ESEP_HAL_LIGHTS_H
#define ESEP_HAL_LIGHTS_H

#include "hal/hal.h"

#include "lib/timer.h"

namespace esep
{
	namespace hal
	{
		class Lights
		{
			private:
			enum class State
			{
				ON,
				OFF,
				FLASHING_ON,
				FLASHING_OFF
			};

			class SingleLight
			{
				public:
					SingleLight(HAL *hal, uint32_t l)
						: mHAL(hal)
						, mLight(l)
						, mState(State::OFF)
							{ }
					~SingleLight( ) { removeTimer(); }
					void turnOn( );
					void turnOff( );
					void flash(uint);
				private:
					void removeTimer( );
					void doTurnOn( );
					void doTurnOff( );

				private:
					HAL * const mHAL;
					uint32_t mLight;
					State mState;
					lib::Timer::Class::TimerManager mTimer;
			};

			public:
			enum class Light : uint32_t
			{
				GREEN  = (1 << 18),
				YELLOW = (1 << 17),
				RED    = (1 << 16)
			};

			public:
				Lights(HAL *);
				~Lights( );
				void turnOn(Light);
				void turnOff(Light);
				void flash(Light, uint);

			private:
				std::map<Light, SingleLight *> mLights;
		};
	}
}

#endif

