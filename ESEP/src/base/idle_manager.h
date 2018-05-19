#ifndef SRC_BASE_IDLE_MANAGER_H
#define SRC_BASE_IDLE_MANAGER_H

#include <memory>

#include "base/IManager.h"

#include "communication/packet.h"
#include "communication/IRecipient.h"

#include "lib/timer.h"

namespace esep
{
	namespace base
	{
		class IdleManager : public IManager
		{
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Message Message;
			typedef Packet::Location Location;

			static constexpr uint T_MIN = 5;
			static constexpr uint T_MAX = 1000;

			public:
				IdleManager(communication::IRecipient *);
				void handle(Event);
				void accept(Packet_ptr) { }

			private:
				communication::IRecipient * const mBaseHandler;
				uint mTime;
		};
	}
}

#endif
