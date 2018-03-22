#ifndef ESEP_HAL_BUTTONS_H
#define ESEP_HAL_BUTTONS_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Buttons
		{
			public:
			enum class Button : uint8_t
			{
				START = 0x10, // Bit 4
				STOP  = 0x20, // Bit 5
				RESET = 0x40, // Bit 6
				ESTOP = 0x80  // Bit 7
			};

			public:
				Buttons(HAL *);
				bool isPressed(Button);
			private:
				HAL * const mHAL;
		};
	}
}

#endif

