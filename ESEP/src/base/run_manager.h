#ifndef SRC_BASE_RUN_MANAGER_H
#define SRC_BASE_RUN_MANAGER_H

#include "communication/packet.h"
#include "IManager.h"
#include "lib/enum.h"
#include "hal/hal.h"
#include "lib/petri_net.h"

namespace esep
{
	namespace base
	{
		class RunManager : public IManager
		{
			private:
			enum class Auto {FIRE};
			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Packet::msg_t msg_t;
			typedef communication::Message Message;
			typedef communication::Packet::Location Location;
			typedef hal::HAL::Event Event;
			typedef tml::MakeCompoundEnum<Event, Message::Run, Auto> petriEvents_t;
			typedef lib::PetriNet<petriEvents_t> petri_t;

			public:
			   RunManager(communication::IRecipient *);
			   ~RunManager();
			   void enter() override;
			   void leave() override;
			   void handle(Event) override;
			   void accept(Packet_ptr) override;

			private:
			   void initLogic();

			private:
			   communication::IRecipient * mMaster;
			   petri_t mLogic;
		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
