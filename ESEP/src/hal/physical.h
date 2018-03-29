#ifndef ESEP_HAL_PHYSICAL_H
#define ESEP_HAL_PHYSICAL_H

#include "lib/utils.h"
#include "hal/buffer.h"

namespace esep
{
	namespace hal
	{
		class Physical : public Buffer
		{
			using HAL::Port;

			public:
				void out8(Port, uint8_t);
			private:
		};
	}
}

#endif

