#ifndef ESEP_HAL_HEIGHTSENSOR_H
#define ESEP_HAL_HEIGHTSENSOR_H

#include "hal/hal.h"
#include "base/config_object.h"

namespace esep
{
	namespace hal
	{
		class HeightSensor
		{
			typedef base::ConfigObject ConfigObject;

			public:
				HeightSensor(HAL *, ConfigObject *);
				uint16_t measure(bool normalize = true);
				bool isValid( );

			private:
				uint16_t measureNormalized(uint16_t);

			private:
				HAL * const mHAL;
				ConfigObject * const mConfig;
		};
	}
}

#endif

