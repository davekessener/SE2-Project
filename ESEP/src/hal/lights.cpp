#include "hal/lights.h"

namespace esep { namespace hal {

Lights::Lights(HAL *hal)
	: mHAL(hal)
{
}

void Lights::turnOn(Light l)
{
	mHAL->set(HAL::Field::GPIO_1, static_cast<uint32_t>(l));
}

void Lights::turnOff(Light l)
{
	mHAL->reset(HAL::Field::GPIO_1, static_cast<uint32_t>(l));
}

void Lights::flash(Light l, uint p)
{
	// TODO timer
}

}}

