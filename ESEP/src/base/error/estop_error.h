
#ifndef SRC_BASE_ERROR_ESTOP_ERROR_H_
#define SRC_BASE_ERROR_ESTOP_ERROR_H_

#include "base/error/recoverable_error.h"

namespace esep
{
	namespace base
	{
		class EstopError : public IRecoverableError
		{
			typedef hal::Buttons::Button Button;
			typedef hal::Lights::Light Light;
			typedef hal::HAL::Event Event;
			typedef hal::LEDs::LED LED;
			typedef communication::Packet::Message Message;
			typedef communication::Packet::Location Location;

			public:
				EstopError(communication::IRecipient *);
				void handle(Event) override;
				void accept(std::shared_ptr<communication::Packet>) override { };
		};
}
}

#endif /* SRC_BASE_ERROR_ESTOP_ERROR_H_ */
