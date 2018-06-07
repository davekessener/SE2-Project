#ifndef SRC_BASE_RUN_MANAGER_H
#define SRC_BASE_RUN_MANAGER_H

#include <memory>
#include <tuple>
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
#include "data/heightmap_data.h"

namespace esep
{
	namespace base
	{
		class RunManager : public IManager
		{
			public:
			MXT_DEFINE_E(UndefinedEventException);
			MXT_DEFINE_E(NoMeasuredHighMap);

			private:
			enum class Auto : uint8_t {FIRE};

			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Packet::msg_t msg_t;
			typedef communication::Message Message;
			typedef communication::Packet::Location Location;
			typedef data::RunManagerTimer::TimerEvent TimerEvent;
			typedef hal::LightBarriers::LightBarrier LightBarrier;
			typedef run::State State;
			typedef tml::MakeCompoundEnum<run::HalEvent, Message::Run, TimerEvent, Auto> petriEvents_t;
			typedef lib::PetriNet<petriEvents_t> petri_t;
			typedef lib::Timer Timer;
			typedef uint64_t timeStamp_t;
			typedef std::shared_ptr<data::HeightMap> heightMap_ptr;
			// bool is for checking if the heightmap is finished
			typedef std::deque<std::tuple<bool, timeStamp_t, heightMap_ptr>> buffer_t;


			public:
			   RunManager(communication::IRecipient *, ConfigObject *);
			   ~RunManager();
			   void enter() override;
			   void leave() override;
			   void handle(Event) override;
			   void accept(Packet_ptr) override;

			private:
			   void initLogic();
			   uint32_t computeMinTime(uint32_t time);
			   uint32_t computeMaxTime(uint32_t time);
			   void takeMeasurement();
			   void takeTimerEvent(TimerEvent);
			   uint64_t timeDiff(uint64_t old, uint64_t curr);
			   void sendMasterMessage(Message::Run msg);
			   void sendMessageWithData(Location target, Message::Run msg, data::Data_ptr data);
			   void sendItemInfo(data::Data_ptr, data::Data_ptr);

			private:
			   communication::IRecipient * mMaster;
			   ConfigObject * mConfig;
			   run::TimeCtrl mTimeCtrl;
			   petri_t mLogic;
			   buffer_t mHeightMapBuffer;
		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
