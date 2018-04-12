#ifndef ESEP_HAL_GPIO_H
#define ESEP_HAL_GPIO_H

#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		class GPIO
		{
			public:
				explicit GPIO(uint32_t);
				~GPIO( );
				void configure(uint32_t);
				void write(uint32_t);
				uint32_t read( );
				void setBits(uint32_t);
				void resetBits(uint32_t);
				void configureInt( );
				void enableInt( );
				void disableInt( );
				void clearIntFlags( );
			private:
				const uint32_t mBaseAddr;

				GPIO(const GPIO&) = delete;
				GPIO& operator=(const GPIO&) = delete;
		};
	}
}

#endif
