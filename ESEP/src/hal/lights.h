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
			enum class Light : uint8_t
			{
				GREEN  = 0x20, // Bit 5
				YELLOW = 0x40, // Bit 6
				RED    = 0x80  // Bit 7
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

