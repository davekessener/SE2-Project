#include "hal/height_sensor.h"

#define MXT_BM_VALID (1 << 4)
#define MXT_QUARTER (1/4)

namespace esep { namespace hal {

HeightSensor::HeightSensor(HAL *hal, ConfigObject *config)
	: mHAL(hal)
	, mConfig(config)
{
}

uint16_t HeightSensor::measure(void)
{
	if (mConfig->isValid())
	{
		return measureNormalized(mHAL->in(HAL::Field::ANALOG));
	}
	else
	{
		return mHAL->in(HAL::Field::ANALOG);
	}
}

bool HeightSensor::isValid(void)
{
	return mHAL->in(HAL::Field::GPIO_0) & MXT_BM_VALID;
}

uint16_t HeightSensor::measureNormalized(uint16_t val)
{
	uint16_t min = mConfig->heightSensorMin();
	uint16_t max = mConfig->heightSensorMax();

	uint16_t lowerLimit = min + MXT_QUARTERT * max;
	uint16_t upperLimit = max;

	if (val < upperLimit)
	{
		return upperLimit;
	}
	else if (val > lowerLimit)
	{
		return lowerLimit;
	}
	else
	{
		return val;
	}

}

}}

