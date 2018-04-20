#ifndef ESEP_QNX_CHANNEL_H
#define ESEP_QNX_CHANNEL_H

#include "lib/utils.h"
#include "qnx/pulse.h"
#include "qnx/connection.h"
#include "hal/gpio.h"

namespace esep
{
	namespace qnx
	{
		class Channel
		{
			public:
				Channel(uint32_t = 0);
				~Channel( );
				Connection connect( );
				pulse_t receivePulse( );
				void registerInterruptListener(Connection&, hal::GPIO&, int8_t);
				void registerTimerListener(Connection&, int8_t, uint64_t);
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
