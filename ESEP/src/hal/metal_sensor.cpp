#include "hal/metal_sensor.h"

#define MXT_BM_ISMETAL (1 << 7)

namespace esep { namespace hal {

MetalSensor::MetalSensor(HAL *hal)
	: mHAL(hal)
{
}

bool MetalSensor::isMetal(void) const
{
	return mHAL->in(HAL::Field::GPIO_0) & MXT_BM_ISMETAL;
}

}}

