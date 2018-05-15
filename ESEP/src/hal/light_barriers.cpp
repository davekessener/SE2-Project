#include "hal/light_barriers.h"

namespace esep { namespace hal {

LightBarriers::LightBarriers(HAL *hal)
	: mHAL(hal)
{
}

bool LightBarriers::isBroken(LightBarrier lb)
{
	return !(mHAL->in(HAL::Field::GPIO_0) & static_cast<uint32_t>(lb));
}

}}

