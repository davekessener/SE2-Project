
#ifndef SRC_BASE_ERROR_RAMP_ERROR_H_
#define SRC_BASE_ERROR_RAMP_ERROR_H_

#include "base/error/recoverable_error.h"
#include "hal/light_barriers.h"

namespace esep
{
	namespace base
	{
		class RampError : public IRecoverableError
		{
			typedef hal::LightBarriers::LightBarrier LightBarrier;
			typedef hal::Buttons::Button Button;
			typedef hal::Lights::Light Light;
			typedef hal::HAL::Event Event;
			typedef hal::LEDs::LED LED;
			typedef communication::Packet::Message Message;
			typedef communication::Packet::Location Location;

			static constexpr uint FIVEHNDRT_mHZ = 2000;

			public:
				RampError(communication::IRecipient *);
				void handle(Event) override;
				void accept(std::shared_ptr<communication::Packet>) override { };

			private:
				hal::LightBarriers& LIGHT_BARRIERS;

		};
}
}
#endif /* SRC_BASE_ERROR_RAMP_ERROR_H_ */
