#ifndef ESEP_QNX_CHANNEL_H
#define ESEP_QNX_CHANNEL_H

#include "lib/utils.h"

#include "qnx/pulse.h"
#include "qnx/connection.h"

#include "hal/gpio.h"
#include "hal/adc/adc.h"

namespace esep
{
	namespace qnx
	{
		class Channel
		{
			typedef hal::adc::ADC ADC;

			public:
			MXT_DEFINE_E(TimerRedefinedException);

			public:
				Channel(uint32_t = 0);
				~Channel( );
				Connection connect( );
				pulse_t receivePulse( );

				// registers a connection to be notified by sending a pulse
				// message with an int8_t code whenever the passed GPIO object
				// causes an interrupt
				void registerInterruptListener(Connection&, hal::GPIO&, int8_t);

				// schedules a connection to be notified via pulse message.
				// accepts the int8_t code to be send as well as the callbacks
				// frequency in ns in a uint64_t
				void registerTimerListener(Connection&, int8_t, uint64_t);

				void registerADC(Connection&, ADC&, int8_t);

			private:
				channel_id_t mID;
				int mIntID = INVALID_ID;
				int8_t mIntCode = -1;

				Channel(const Channel&) = delete;
				Channel& operator=(const Channel&) = delete;
		};
	}
}

#endif
