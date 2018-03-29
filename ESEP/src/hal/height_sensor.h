#ifndef ESEP_HAL_HEIGHTSENSOR_H
#define ESEP_HAL_HEIGHTSENSOR_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class HeightSensor
		{
			public:
				HeightSensor(HAL *);
				uint16_t measure( );
				bool isValid( );
			private:
				HAL * const mHAL;
		};
	}
}

#endif

