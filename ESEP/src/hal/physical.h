#ifndef ESEP_HAL_PHYSICAL_H
#define ESEP_HAL_PHYSICAL_H

#include <atomic>
#include <memory>

#include "lib/utils.h"
#include "lib/thread.h"
#include "hal/buffer.h"
#include "hal/gpio.h"
#include "hal/adc/ADC.h"
#include "qnx/connection.h"
#include "lib/timer.h"

#define MXT_NGPIOS 3

namespace esep
{
	namespace hal
	{
		class Physical : public Buffer
		{
			private:
			/**
			 * Codes that the Physical object can receive
			 * in pulse messages.
			 * For internal use only
			 */
			enum class Code : int8_t
			{
				SHUTDOWN,
				INTERRUPT,
				GPIO_1_OUT,
				GPIO_1_SET,
				GPIO_1_RESET,
				GPIO_2_OUT,
				GPIO_2_SET,
				GPIO_2_RESET,
				ANALOG_SET
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
				void updateADC(uint32_t);
				void onGPIO(uint, gpio_fn, uint32_t);
				void sendGPIOPulse(Field, uint, uint);

			private:
				std::unique_ptr<GPIO> mGPIOs[MXT_NGPIOS];
				std::unique_ptr<adc::ADC> mADC;
				lib::Thread mHALThread;
				std::atomic<bool> mRunning, mThreadAlive;
				qnx::Connection mConnection;
		};
	}
}

#endif

