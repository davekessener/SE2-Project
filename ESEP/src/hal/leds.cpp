#include "hal/leds.h"

namespace esep { namespace hal {

LEDs::LEDs(HAL *hal)
	: mHAL(hal)
	, mStatus(0)
{
}

void LEDs::turnOn(LED led)
{
	mStatus |= static_cast<uint32_t>(led);
	mHAL->set(HAL::Field::GPIO_2, static_cast<uint32_t>(led));
}

void LEDs::turnOff(LED led)
{
	mStatus &= ~static_cast<uint32_t>(led);
	mHAL->reset(HAL::Field::GPIO_2, static_cast<uint32_t>(led));
}

}}

