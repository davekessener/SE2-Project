#include "hal/metal_sensor.h"

#define MXT_BM_ISMETAL (1 << 7)

namespace esep { namespace hal {

MetalSensor::MetalSensor(HAL *hal)
	: mHAL(hal)
{
}

bool MetalSensor::isMetal(void)
{
	return mHAL->in(HAL::Port::B) & MXT_BM_ISMETAL;
}

}}

