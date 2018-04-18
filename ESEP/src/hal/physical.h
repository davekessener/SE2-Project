#ifndef ESEP_HAL_PHYSICAL_H
#define ESEP_HAL_PHYSICAL_H

#include <thread>
#include <atomic>

#include "lib/utils.h"
#include "hal/buffer.h"
#include "hal/gpio.h"
#include "qnx/connection.h"

#define MXT_NGPIOS 3

namespace esep
{
	namespace hal
	{
		class Physical : public Buffer
		{
			enum class Code : int8_t
			{
				SHUTDOWN,
				INTERRUPT,
				GPIO_1_OUT,
				GPIO_1_SET,
				GPIO_1_RESET,
				GPIO_2_OUT,
				GPIO_2_SET,
				GPIO_2_RESET
			};

			typedef void (GPIO::*gpio_fn)(uint32_t);

			public:
			using HAL::Field;
			using HAL::bitmask_t;

			public:
				Physical( );
				~Physical( );
				void out(Field, uint32_t);
				void set(Field, bitmask_t);
				void reset(Field, bitmask_t);
			private:
				void updateSensors( );
				void onGPIO(uint, gpio_fn, uint32_t);
				void sendGPIOPulse(Field, uint, uint);

			private:
				GPIO *mGPIOs[MXT_NGPIOS];
				std::thread mHALThread;
				std::atomic<bool> mRunning;
				qnx::Connection mConnection;
		};
	}
}

#endif

