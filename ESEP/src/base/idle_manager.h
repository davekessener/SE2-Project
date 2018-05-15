#ifndef SRC_BASE_IDLE_MANAGER_H
#define SRC_BASE_IDLE_MANAGER_H

#include <memory>

#include "base/IManager.h"

#include "communication/packet.h"
#include "communication/IRecipient.h"

#include "hal/hal.h"
#include "lib/timer.h"
#include "hal/buttons.h"

namespace esep
{
	namespace base
	{
		class IdleManager : public IManager
		{
			typedef communication::Packet::Location Location;
			typedef communication::Packet::Message Message;

			static constexpr uint T_MIN = 5;
			static constexpr uint T_MAX = 2000;

			public:
				IdleManager(communication::IRecipient *);
				void handle(hal::HAL::Event);
				void accept(communication::Packet_ptr) { }

			private:
				communication::IRecipient * const mBaseHandler;
				int mTime;
				hal::Buttons& BUTTONS;
		};
	}
}

#endif
