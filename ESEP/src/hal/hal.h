#ifndef ESEP_HAL_HAL_H
#define ESEP_HAL_HAL_H

#include <functional>

#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		class HAL
		{
			public:
			typedef uint32_t bitmask_t;

			enum class Field : uint
			{
				GPIO_0 = 0,
				GPIO_1 = 1,
				GPIO_2 = 2,
				ANALOG = 3
			};

			static const uint N_FIELDS = 4;

			enum class Event : uint
			{
				LB_START = 2,
				LB_END = 20,
				LB_HEIGHTSENSOR = 3,
				LB_RAMP = 15,
				LB_SWITCH = 5,
				BTN_START = 22,
				BTN_STOP = 23,
				BTN_RESET = 26,
				BTN_ESTOP = 27
			};

			static const Event EVENTS[];
			static constexpr uint N_EVENTS = 9;

			typedef std::function<void(Event, uint32_t)> callback_t;

			public:
				virtual ~HAL( ) { }
				virtual uint32_t in(Field) = 0;
				virtual void out(Field, uint32_t) = 0;
				virtual void setCallback(callback_t) = 0;
				virtual void set(Field, bitmask_t) = 0;
				virtual void reset(Field, bitmask_t) = 0;
			private:
		};
	}
}

#endif

