#include "hal/height_sensor.h"

#define MXT_BM_VALID 0x04 // 0b00000100

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
	return mHAL->in(HAL::Port::B) & MXT_BM_VALID;
}

}}

