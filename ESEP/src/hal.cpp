#include "hal.h"

#include "lib/logger.h"

namespace esep { namespace hal {

Holder::Holder(void)
	: mHAL(nullptr)
{
}

void Holder::instantiate(HAL *hal, base::ConfigObject *config)
{
	if(mHAL)
	{
		MXT_THROW_EX(HALAlreadyInstantiatedException);
	}

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

void Holder::clear(void)
{
	if(mHAL)
	{
		mHALObjects.reset();

		mHAL = nullptr;
	}
}

}}
