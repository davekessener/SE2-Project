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
				START = 0x01, // Bit 0
				RESET = 0x02, // Bit 1
				Q1    = 0x04, // Bit 2
				Q2    = 0x08  // Bit 3
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

