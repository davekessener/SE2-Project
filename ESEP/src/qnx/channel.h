#ifndef ESEP_QNX_CHANNEL_H
#define ESEP_QNX_CHANNEL_H

#include "lib/utils.h"
#include "qnx/pulse.h"
#include "qnx/connection.h"

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
				void listenForInterrupts(Connection&);
			private:
				channel_id_t mID;
				int mIntID = INVALID_ID;

				Channel(const Channel&) = delete;
				Channel& operator=(const Channel&) = delete;
		};
	}
}

#endif
