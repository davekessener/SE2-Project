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
			enum class LightBarrier : uint32_t
			{
				LB_START		= (1 << 2),
				LB_HEIGHTSENSOR = (1 << 3),
				LB_SWITCH		= (1 << 5),
				LB_RAMP			= (1 << 15),
				LB_END			= (1 << 20)
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

