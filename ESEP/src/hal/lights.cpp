#include "hal/lights.h"

namespace esep { namespace hal {

Lights::Lights(HAL *hal)
	: mHAL(hal)
{
}

void Lights::turnOn(Light l)
{
	mHAL->setBits(HAL::Port::A, static_cast<uint32_t>(l));
}

void Lights::turnOff(Light l)
{
	mHAL->resetBits(HAL::Port::A, static_cast<uint32_t>(l));
}

void Lights::flash(Light l, uint p)
{
	// TODO timer
}

}}

