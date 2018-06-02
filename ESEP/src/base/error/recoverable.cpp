#include "base/error/recoverable.h"

#include "hal.h"

namespace esep { namespace base { namespace error {

typedef hal::Lights::Light Light;
typedef hal::LEDs::LED LED;

Recoverable::Recoverable(communication::IRecipient *handler)
	: mHandler(handler)
	, mState(State::PENDING)
{
}

void Recoverable::enter(void)
{
	Base::enter();

	HAL_LIGHTS.flash(Light::RED, ONE_HZ);
	HAL_LEDS.turnOn(LED::RESET);
}

void Recoverable::leave(void)
{
	HAL_LEDS.turnOff(LED::RESET);
	HAL_LIGHTS.turnOff(Light::RED);
}

void Recoverable::solve(void)
{
	switch(mState)
	{
	case State::PENDING:
		mState = State::GONE;
		break;

	case State::ACK:
		OK();
		break;

	default:
		break;
	}
}

void Recoverable::acknowledge(void)
{
	switch(mState)
	{
	case State::PENDING:
		mState = State::ACK;
		break;

	case State::GONE:
		OK();
		break;

	default:
		break;
	}
}

void Recoverable::OK(void)
{
	mState = State::OK;

	mHandler->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::FIXED));
}

}}}
