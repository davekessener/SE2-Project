#ifndef SRC_BASE_ERROR_ESTOP_ERROR_H
#define SRC_BASE_ERROR_ESTOP_ERROR_H

#include "base/error/recoverable.h"

#include "hal.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Estop : public Recoverable
			{
				typedef hal::Buttons::Button Button;

				public:
					Estop(communication::IRecipient *);

					void handle(Event) override;

					void enter( ) override;
					void accept(communication::Packet_ptr) override { };

					int priority( ) const override { return mPriority; }

				private:
					const int mPriority;
			};
		}
	}
}

#endif
