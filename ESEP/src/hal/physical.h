#ifndef ESEP_HAL_PHYSICAL_H
#define ESEP_HAL_PHYSICAL_H

#include "lib/utils.h"
#include "hal/buffer.h"

#define MXT_NPORTS 3

namespace esep
{
	namespace hal
	{
		class Physical : public Buffer
		{
			using HAL::Port;

			public:
				Physical( );
				void out(Port, uint32_t);
				void subscribeEvent(Event, callback_t);
				void setBits(Port, uint32_t);
				void resetBits(Port, uint32_t);
			private:
				uintptr_t mBaseAddr[MXT_NPORTS];
		};
	}
}

#endif

