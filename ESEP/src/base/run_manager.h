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
#include "hal/light_barriers.h"
#include "data/data_point.h"
#include "data/location_data.h"

namespace esep
{
	namespace base
	{
		class RunManager : public IManager
		{
			public:
			MXT_DEFINE_E(UndefinedEventException);

			private:
			enum class Auto : uint8_t {FIRE};

			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Packet::msg_t msg_t;
			typedef communication::Message::Run runMessage_t;
			typedef communication::Packet::Location Location;
			typedef hal::HAL::Event halEvent_t;
			typedef hal::LightBarriers::LightBarrier LightBarrier;
			typedef run::State State;
			typedef tml::MakeCompoundEnum<run::HalEvent, runMessage_t, run::TimerEvent, Auto> petriEvents_t;
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
			   uint32_t computeMinTime(uint32_t time);
			   uint32_t computeMaxTime(uint32_t time);
			   void sendErrorMessage(runMessage_t msg, data::Location::Type location);

			private:
			   communication::IRecipient * mMaster;
			   ConfigObject * mConfigData;
			   run::TimeCtrl mTimeCtrl;
			   petri_t mLogic;

		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
