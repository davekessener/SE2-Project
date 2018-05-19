#ifndef SRC_BASE_ERROR_RAMP_ERROR_H
#define SRC_BASE_ERROR_RAMP_ERROR_H

#include "base/error/reset_ack.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Ramp : public ResetAck
			{
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef Packet::Location Location;

				static constexpr uint T_PERIOD = 2000;

				public:
					Ramp(communication::IRecipient *);

					void handle(Event) override;

					void accept(Packet_ptr) override { };
			};
		}
	}
}

#endif
