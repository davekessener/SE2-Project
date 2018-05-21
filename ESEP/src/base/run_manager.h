#ifndef SRC_BASE_RUN_MANAGER_H
#define SRC_BASE_RUN_MANAGER_H

#include "communication/packet.h"
#include "IManager.h"
#include "lib/enum.h"
#include "hal/hal.h"


namespace esep
{
	namespace base
	{
		class RunManager : public IManager
		{
			private:
			typedef enum class Auto {FIRE} auto_t;
			typedef communication::Packet::msg_t message;
			typedef hal::HAL::Event hal_e;
			typedef tml::MakeCompoundEnum<hal_e, communication::Message::Run, auto_t> petriEvents;
			//typedef lib::PetriNet<petriEvents> petri_t;

			public:
			   RunManager(communication::IRecipient *);
			   ~RunManager();
			   void enter() override;
			   void leave() override;
			   void handle(hal::HAL::Event) override;
			   void accept(communication::Packet_ptr) override;

			private:
			   communication::IRecipient * mMaster;
			   //petri_t mPetriNet;
		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
