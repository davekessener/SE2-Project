#include "base/error/warning.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::Lights::Light Light;

Warning::Warning(communication::IRecipient *handler)
	: mHandler(handler)
{
}

void Warning::enter()
{
	Base::enter();

	HAL_LIGHTS.flash(Light::RED, ONE_HZ);
	HAL_LIGHTS.turnOn(Light::YELLOW);

	mHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::FIXED));
}

void Warning::leave(void)
{
	HAL_LIGHTS.turnOff(Light::YELLOW);
	HAL_LIGHTS.turnOff(Light::RED);
}

}}}
