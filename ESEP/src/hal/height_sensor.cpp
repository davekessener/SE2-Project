#include "hal/height_sensor.h"

#define MXT_BM_VALID (1 << 4)
#define MXT_MAX 0xFFFF

namespace esep { namespace hal {

HeightSensor::HeightSensor(HAL *hal, ConfigObject *config)
	: mHAL(hal)
	, mConfig(config)
{
}

uint16_t HeightSensor::measure(bool normalize)
{
	if (mConfig->isValid() && normalize)
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
	uint16_t min_sensor = mConfig->heightSensorMin(); min_sensor = -min_sensor;
	uint16_t max_sensor = mConfig->heightSensorMax(); max_sensor = -max_sensor;
	uint16_t range = max_sensor - min_sensor, d = range / 5;

	min_sensor = min_sensor <= MXT_MAX - d ? min_sensor + d : MXT_MAX;
	max_sensor = max_sensor <= MXT_MAX - d ? max_sensor + d : MXT_MAX;
	range = max_sensor - min_sensor;

	uint16_t r = 0;

	val = -val;

	if (val < min_sensor)
	{
		r = 0;
	}
	else if (val > max_sensor)
	{
		r = MXT_MAX;
	}
	else
	{
		r = (val - min_sensor) * MXT_MAX / range ;
	}

	return r;
}

}}

