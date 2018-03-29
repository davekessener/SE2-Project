#ifndef ESEP_HAL_LIGHTBARRIERS_H
#define ESEP_HAL_LIGHTBARRIERS_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class LightBarriers
		{
			public:
			enum class LightBarrier : uint8_t
			{
				LB_START		= 0x01, // Bit 0
				LB_HEIGHTSENSOR = 0x02, // Bit 1
				LB_SWITCH		= 0x08, // Bit 3
				LB_RAMP			= 0x40, // Bit 6
				LB_END			= 0x80  // Bit 8
			};

			public:
				LightBarriers(HAL *);
				bool isBroken(LightBarrier);
			private:
				HAL * const mHAL;
		};
	}
}

#endif

