
#ifndef SRC_BASE_ERROR_IRRECOVERABLE_ERROR_H_
#define SRC_BASE_ERROR_IRRECOVERABLE_ERROR_H_

#include "base/IManager.h"
#include "hal/hal.h"
#include "hal/lights.h"
#include "hal/motor.h"
#include "hal/switch.h"

namespace esep
{
	namespace base
	{
		class IrrecoverableError : public IManager
		{
			typedef communication::Packet::Message Message;
			typedef hal::Lights::Light Light;

			public:
				IrrecoverableError(communication::IRecipient *, Message);
				void enter() override;
				void leave() override { };
				void handle(hal::HAL::Event) override { };
				void accept(std::shared_ptr<communication::Packet>) override { };

			private:
				communication::IRecipient * const mHandler;
				hal::Switch& SWITCH;
				hal::Lights& LIGHTS;
				hal::Motor& MOTOR;
				Message mMessage;
		};
}
}
#endif
