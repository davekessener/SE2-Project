#ifndef ESEP_HAL_METALSENSOR_H
#define ESEP_HAL_METALSENSOR_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class MetalSensor
		{
			public:
				MetalSensor(HAL *);
				bool isMetal( );
			private:
				HAL * const mHAL;
		};
	}
}

#endif

