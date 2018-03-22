#include "hal\metal_sensor.h"

#define MXT_BM_ISMETAL 0x10 // 0b00010000

namespace esep { namespace hal {

MetalSensor::MetalSensor(HAL *hal)
	: mHAL(hal)
{
}

bool MetalSensor::isMetal(void)
{
	return mHAL->in8(HAL::Port::B) & MXT_BM_ISMETAL;
}

}}

