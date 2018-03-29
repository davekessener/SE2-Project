#include "hal/leds.h"

namespace esep { namespace hal {

LEDs::LEDs(HAL *hal)
	: mHAL(hal)
{
}

void LEDs::turnOn(LED led)
{
	mStatus |= static_cast<uint8_t>(led);
	mHAL->setBits(HAL::Port::C, mStatus);
}

void LEDs::turnOff(LED led)
{
	mStatus &= ~static_cast<uint8_t>(led);
	mHAL->resetBits(HAL::Port::C, static_cast<uint8_t>(led));
}

}}

