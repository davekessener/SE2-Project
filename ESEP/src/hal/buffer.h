#ifndef ESEP_HAL_BUFFER_H
#define ESEP_HAL_BUFFER_H

#include "lib/utils.h"

#include "hal/hal.h"

#define MXT_N_PORTS 3

namespace esep
{
	namespace hal
	{
		class Buffer : public HAL
		{
			public:
			public:
				uint32_t in(Port p) { return mPorts[static_cast<uint>(p)]; }
				uint16_t inAnalog( ) { return mAnalog; }
				virtual void update(Field, uint32_t);
			private:
				uint32_t mPorts[MXT_N_PORTS];
				uint16_t mAnalog;
		};
	}
}

#endif

