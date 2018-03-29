#include "hal/buttons.h"

#define MXT_BM_STOP (static_cast<uint8_t>(Button::STOP)|static_cast<uint8_t>(Button::ESTOP))

namespace esep { namespace hal {

Buttons::Buttons(HAL *hal)
	: mHAL(hal)
{
}

bool Buttons::isPressed(Button b)
{
	return (mHAL->in8(HAL::Port::C) ^ MXT_BM_STOP) & static_cast<uint8_t>(b);
}

}}

