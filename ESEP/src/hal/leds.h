#ifndef ESEP_HAL_LEDS_H
#define ESEP_HAL_LEDS_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class LEDs
		{
			public:
			enum class LED : uint8_t
			{
				START = (1 << 2),
				RESET = (1 << 3),
				Q1    = (1 << 4),
				Q2    = (1 << 5)
			};

			public:
				LEDs(HAL *);
				void turnOn(LED);
				void turnOff(LED);
				bool isOn(LED led) const { return mStatus & static_cast<uint8_t>(led); }
			private:
				HAL * const mHAL;
				uint8_t mStatus;
		};
	}
}

#endif

