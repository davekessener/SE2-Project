#include "hal/height_sensor.h"

#define MXT_BM_VALID (1 << 4)

namespace esep { namespace hal {

HeightSensor::HeightSensor(HAL *hal)
	: mHAL(hal)
{
}

uint16_t HeightSensor::measure(void)
{
	return 0; // TODO
}

bool HeightSensor::isValid(void)
{
	return mHAL->in(HAL::Field::GPIO_0) & MXT_BM_VALID;
}

}}

