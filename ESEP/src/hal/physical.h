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
				void onInt( );
				void onGPIO(uint, uint, uint);
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

