#ifndef SRC_BASE_ERROR_ESTOP_ERROR_H
#define SRC_BASE_ERROR_ESTOP_ERROR_H

#include "base/error/recoverable.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Estop : public Recoverable
			{
				public:
					Estop(communication::IRecipient *);

					void handle(Event) override;

					void enter( ) override;
					void accept(communication::Packet_ptr) override { };

					int priority( ) const override { return 9; }
			};
		}
	}
}

#endif
