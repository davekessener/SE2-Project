#include "hal/lights.h"

#include "lib/logger.h"

namespace esep { namespace hal {

Lights::Lights(HAL *hal)
{
	auto createLight = [this, hal](Light l) {
		mLights.emplace(std::make_pair(l, SingleLight(hal, static_cast<uint32_t>(l))));
	};

	createLight(Light::RED);
	createLight(Light::GREEN);
	createLight(Light::YELLOW);
}

Lights::~Lights(void)
{
}

void Lights::turnOn(Light l)
{
	mLights.at(l).turnOn();
}

void Lights::turnOff(Light l)
{
	mLights.at(l).turnOff();
}

void Lights::flash(Light l, uint p)
{
	mLights.at(l).flash(p);
}

// # --------------------------------------------------------------------------

void Lights::SingleLight::flash(uint p)
{
	removeTimer();

	doTurnOn();
	mState = State::FLASHING_ON;
	mTimer = lib::Timer::instance().registerCallback([this](void) -> bool {
		switch(mState)
		{
		case State::FLASHING_ON:
			doTurnOff();
			mState = State::FLASHING_OFF;
			break;
		case State::FLASHING_OFF:
			doTurnOn();
			mState = State::FLASHING_ON;
			break;
		default:
			MXT_LOG("This should not happen!");
			return false;
		}

		return true;
	}, p / 2, p / 2);
}

void Lights::SingleLight::turnOn(void)
{
	removeTimer();
	mState = State::ON;
	doTurnOn();
}

void Lights::SingleLight::turnOff(void)
{
	removeTimer();
	mState = State::OFF;
	doTurnOff();
}

void Lights::SingleLight::doTurnOn(void)
{
	mHAL->set(HAL::Field::GPIO_1, mLight);
}

void Lights::SingleLight::doTurnOff(void)
{
	mHAL->reset(HAL::Field::GPIO_1, mLight);
}

void Lights::SingleLight::removeTimer(void)
{
	if(mTimer != lib::Timer::Class::INVALID_TIMER_ID)
	{
		lib::Timer::instance().unregisterCallback(mTimer);
		mTimer = lib::Timer::Class::INVALID_TIMER_ID;
	}
}

}}

