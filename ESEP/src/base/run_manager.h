#ifndef SRC_BASE_RUN_MANAGER_H
#define SRC_BASE_RUN_MANAGER_H

#include <memory>

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
			typedef communication::Message::Run runMessage_t;
			typedef communication::Packet::Location Location;
			typedef data::RunManagerTimer::TimerEvent TimerEvent;
			typedef hal::LightBarriers::LightBarrier LightBarrier;
			typedef run::State State;
			typedef tml::MakeCompoundEnum<run::HalEvent, runMessage_t, TimerEvent, Auto> petriEvents_t;
			typedef lib::PetriNet<petriEvents_t> petri_t;
			typedef lib::Timer Timer;
			typedef uint64_t timeStamp_t;
			typedef std::shared_ptr<data::HeightMap> heightMap_ptr;
			typedef std::deque<std::pair<timeStamp_t, heightMap_ptr>> buffer_t;


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
			   uint64_t timeDiff(uint64_t old, uint64_t curr);
			   void sendMasterMessage(runMessage_t msg);
			   void sendMessageWithData(Location target, runMessage_t msg, data::Data_ptr data);
			   void sendItemInfo(data::Data_ptr, data::Data_ptr);

			private:
			   communication::IRecipient * mMaster;
			   ConfigObject * mConfigData;
			   run::TimeCtrl mTimeCtrl;
			   petri_t mLogic;
			   buffer_t mHeightMapBuffer;


		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
