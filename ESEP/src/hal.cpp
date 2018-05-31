#include "hal.h"

#include "lib/logger.h"

namespace esep { namespace hal {

Holder::Holder(void)
	: mHAL(nullptr)
{
}

void Holder::instantiate(HAL *hal, base::ConfigObject *config)
{
	mHAL = hal;
	mHALObjects.reset(new hal_t(
		Buttons(mHAL),
		HeightSensor(mHAL, config),
		LEDs(mHAL),
		Switch(mHAL),
		LightBarriers(mHAL),
		Lights(mHAL),
		MetalSensor(mHAL),
		Motor(mHAL)));

	MXT_LOG("New HAL instance");
}

}}
