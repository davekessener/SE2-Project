#include "hal/light_barriers.h"

namespace esep { namespace hal {

LightBarriers::LightBarriers(HAL *hal)
	: mHAL(hal)
{
}

bool LightBarriers::isBroken(LightBarrier lb)
{
	return mHAL->in8(HAL::Port::B) & static_cast<uint8_t>(lb);
}

}}

