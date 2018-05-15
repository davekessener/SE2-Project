#ifndef SRC_BASE_READY_MANAGER_H
#define SRC_BASE_READY_MANAGER_H

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
		class ReadyManager : public IManager
		{
			public:
				ReadyManager(communication::IRecipient *);
				void handle(hal::HAL::Event);
				void accept(communication::Packet_ptr) { }

			private:
				communication::IRecipient * const mBaseHandler;
				hal::Buttons& BUTTONS;
		};
	 }
}

#endif
