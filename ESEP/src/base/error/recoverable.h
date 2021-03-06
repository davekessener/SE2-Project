#ifndef SRC_BASE_ERROR_RECOVERABLE_ERROR_H
#define SRC_BASE_ERROR_RECOVERABLE_ERROR_H

#include "base/error/base.h"

#include "communication/packet.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Recoverable : public Base
			{
				typedef communication::Packet Packet;
				typedef communication::Message Message;
				typedef Packet::Location Location;

				static constexpr uint ONE_HZ = 1000;
				static constexpr uint HALF_HZ = 2000;

				enum class State
				{
					PENDING,
					ACK,
					GONE,
					OK
				};

				public:
					Recoverable(communication::IRecipient *);
					void enter() override;
					void leave() override;

					int priority( ) const override { return 0; }

				protected:
					void solve( );
					void acknowledge( );

				private:
					void OK( );

				private:
					communication::IRecipient * const mHandler;
					State mState;
			};
		}
	}
}

#endif
