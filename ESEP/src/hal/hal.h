#ifndef ESEP_HAL_HAL_H
#define ESEP_HAL_HAL_H

#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		class HAL
		{
			public:
			enum class Port : uint8_t
			{
				A,
				B,
				C
			};

			typedef uint8_t bitmask_t;

			public:
				virtual ~HAL( ) { }
				virtual uint8_t in8(Port) = 0;
				virtual uint16_t in16( ) = 0;
				virtual void out8(Port, uint8_t) = 0;
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

