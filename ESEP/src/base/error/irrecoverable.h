#ifndef SRC_BASE_ERROR_IRRECOVERABLE_ERROR_H
#define SRC_BASE_ERROR_IRRECOVERABLE_ERROR_H

#include "base/error/base.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Irrecoverable : public Base
			{
				typedef communication::Message Message;

				static constexpr uint ONE_HZ = 1000;

				public:
					Irrecoverable(communication::IRecipient *, Message::Error);

					void enter( ) override;
					void leave( ) override;

					void accept(communication::Packet_ptr) override { };

					int priority( ) const { return 10; }

				private:
					communication::IRecipient * const mHandler;
					const Message::Error mMessage;
			};
		}
	}
}

#endif
