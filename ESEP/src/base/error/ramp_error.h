
#ifndef SRC_BASE_ERROR_RAMP_ERROR_H_
#define SRC_BASE_ERROR_RAMP_ERROR_H_

#include "base/IManager.h"
#include "hal/hal.h"
#include "hal/height_sensor.h"
#include "hal/light_barriers.h"
#include "hal/lights.h"
#include "hal/motor.h"
#include "hal/leds.h"
#include "hal/switch.h"

namespace esep
{
	namespace base
	{
		class RampError : public IManager
		{
			public:
				RampError(communication::IRecipient *);
				void enter() override;
				void leave() override;
				void handle(hal::HAL::Event) override;
				void accept(std::shared_ptr<communication::Packet>) override {};

			private:
				communication::IRecipient * const mHandler;
				hal::Switch& SWITCH;
				hal::LightBarriers& LIGHT_BARRIERS;
				hal::Lights& LIGHTS;
				hal::LEDs& LEDS;
				hal::Motor& MOTOR;
		};
}
}
#endif /* SRC_BASE_ERROR_RAMP_ERROR_H_ */
