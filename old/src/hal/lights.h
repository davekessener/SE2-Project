#ifndef ESEP_HAL_LIGHTS_H
#define ESEP_HAL_LIGHTS_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Lights
		{
			public:
			enum class Light : uint32_t
			{
				GREEN  = (1 << 18),
				YELLOW = (1 << 17),
				RED    = (1 << 16)
			};

			public:
				Lights(HAL *);
				void turnOn(Light);
				void turnOff(Light);
				void flash(Light, uint);
			private:
				HAL * const mHAL;
		};
	}
}

#endif

