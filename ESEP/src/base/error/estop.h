#ifndef SRC_BASE_ERROR_ESTOP_ERROR_H
#define SRC_BASE_ERROR_ESTOP_ERROR_H

#include "base/error/reset_ack.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Estop : public ResetAck
			{
				public:
					Estop(communication::IRecipient *);

					void handle(Event) override;

					void enter( ) override;
					void accept(communication::Packet_ptr) override { };
			};
		}
	}
}

#endif
