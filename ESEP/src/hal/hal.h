#ifndef ESEP_HAL_HAL_H
#define ESEP_HAL_HAL_H

#include <functional>

#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		enum class Field : uint
		{
			PORT_A = 0,
			PORT_B = 1,
			PORT_C = 2,
			ANALOG = 3
		};

		class HAL
		{
			public:
			enum class Port : uint
			{
				A,
				B,
				C
			};

			typedef uint8_t bitmask_t;

			enum class Event : uint
			{
				LB_START,
				LB_END,
				LB_HEIGHTSENSOR,
				LB_RAMP,
				LB_SWITCH
			};

			typedef std::function<void(Event)> callback_t;

			public:
				virtual ~HAL( ) { }
				virtual uint8_t in8(Port) = 0;
				virtual uint16_t in16( ) = 0;
				virtual void out8(Port, uint8_t) = 0;
				virtual void subscribeEvent(Event, callback_t) = 0;
				virtual void setBits(Port p, bitmask_t m)
					{ uint8_t v = in8(p); v |= m; out8(p, v); }
				virtual void resetBits(Port p, bitmask_t m)
					{ uint8_t v = in8(p); v &= ~m; out8(p, v); }
				virtual void toggleBits(Port p, bitmask_t m)
					{ uint8_t v = in8(p); v ^= m; out8(p, v); }
			private:
		};
	}
}

#endif

