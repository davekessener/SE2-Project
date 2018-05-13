
#ifndef SRC_BASE_ERROR_RECOVERABLE_ERROR_H_
#define SRC_BASE_ERROR_RECOVERABLE_ERROR_H_

#include "base/IManager.h"
#include "hal/hal.h"
#include "hal/lights.h"
#include "hal/motor.h"
#include "hal/leds.h"
#include "hal/switch.h"
#include "hal/buttons.h"

namespace esep
{
	namespace base
	{
		class IRecoverableError : public IManager
		{
			typedef hal::Lights::Light Light;
			typedef hal::LEDs::LED LED;

			protected:
				enum class State
				{
					PENDING,
					PENDING_ACK,
					GONE
				};

			protected:
				IRecoverableError(communication::IRecipient *);
				void enter() override;
				void leave() override;

			protected:
				communication::IRecipient * const mHandler;
				hal::Switch& SWITCH;
				hal::Lights& LIGHTS;
				hal::LEDs& LEDS;
				hal::Motor& MOTOR;
				hal::Buttons& BUTTONS;
				State mState;
		};
}
}

#endif /* SRC_BASE_ERROR_RECOVERABLE_ERROR_H_ */
