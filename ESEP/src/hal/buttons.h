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
			enum class Button : uint32_t
			{
				START = (1 << 22),
				STOP  = (1 << 23),
				RESET = (1 << 26),
				ESTOP = (1 << 27)
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

