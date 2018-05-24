#ifndef SRC_BASE_RUN_MANAGER_H
#define SRC_BASE_RUN_MANAGER_H

#include "communication/packet.h"
#include "IManager.h"
#include "lib/enum.h"
#include "hal/hal.h"
#include "lib/petri_net.h"
#include "lib/timer.h"
#include "config_object.h"
#include "run/time_controller.h"
#include "run/types.h"

namespace esep
{
	namespace base
	{
		class RunManager : public IManager
		{
			public:

			private:
			enum class Auto : uint8_t {FIRE};

			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Packet::msg_t msg_t;
			typedef communication::Message::Run Message;
			typedef communication::Packet::Location Location;
			typedef hal::HAL::Event Event;
			typedef tml::MakeCompoundEnum<Event, Message, run::TimerEvent, Auto> petriEvents_t;
			typedef lib::PetriNet<petriEvents_t> petri_t;
			typedef lib::Timer Timer;

			public:
			   RunManager(communication::IRecipient *, ConfigObject *);
			   ~RunManager();
			   void enter() override;
			   void leave() override;
			   void handle(Event) override;
			   void accept(Packet_ptr) override;
			   void acceptTimerEvent(run::TimerEvent);

			private:
			   void initLogic();

			private:
			   communication::IRecipient * mMaster;
			   ConfigObject * mConfigData;
			   run::TimeCtrl mTimeCtrl;
			   petri_t mLogic;

		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
