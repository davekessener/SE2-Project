#include "idle_manager.h"

#include "lib/logger.h"

#include "hal.h"

namespace esep { namespace base {

typedef hal::Lights::Light Light;

void IdleManager::enter(void)
{
	if(mConfig->isValid())
	{
		mCom->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::VALID));
	}
}

void IdleManager::leave(void)
{
	HAL_LIGHTS.turnOff(Light::YELLOW);
}

void IdleManager::shortPress(void)
{
	HAL_CONSOLE.println("System has not been configured properly!");
	HAL_LIGHTS.turnOn(Light::YELLOW);
}

}}
