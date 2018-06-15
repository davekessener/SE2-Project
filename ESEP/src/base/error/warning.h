#ifndef SRC_BASE_ERROR_WARNING_ERROR_H
#define SRC_BASE_ERROR_WARNING_ERROR_H

#include "base/error/base.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Warning : public Base
			{
				typedef communication::Message Message;
				typedef communication::Packet Packet;
				typedef communication::Packet_ptr Packet_ptr;
				typedef Packet::Location Location;

				static constexpr uint ONE_HZ = 1000;

				public:
					Warning(communication::IRecipient *);

					void enter() override;
					void leave() override;
					void handle(Event) override { };

					void accept(Packet_ptr) override { };

					int priority( ) const override { return -1; }

				private:
					communication::IRecipient * const mHandler;
			};
		}
	}
}

#endif
