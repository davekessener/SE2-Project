
#ifndef SRC_TEST_UT_RUN_MANAGER_LOGIC_H
#define SRC_TEST_UT_RUN_MANAGER_LOGIC_H

#include <vector>

#include "test/unit/unit.h"
#include "base/run_manager.h"
#include "communication/packet.h"
#include "hal/light_barriers.h"
#include "lib/logger.h"



namespace esep
{
	namespace test
	{
		namespace ut
		{
			class RunManagerLogic : public unit::TestSuite
			{
				struct HandlerDummy;

				typedef base::RunManager::Event Event;
				typedef base::ConfigObject config_t;
				typedef base::RunManager RunManager;
				typedef data::RunManagerTimer::TimerEvent TimerEvent;
				typedef communication::Packet Packet;
				typedef communication::Packet::Location Location;
				typedef communication::Message Message;
				typedef communication::Packet::msg_t msg_t;
				typedef communication::Packet_ptr Packet_ptr;
				typedef hal::LightBarriers::LightBarrier LightBarrier;
				typedef hal::HAL::Field Field;


				public:
				   RunManagerLogic();

				protected:
				   void setup( );
				   void teardown( );
				   void define( );

				private:
				   void sendPacket(msg_t);
				   uint32_t maxTime(uint32_t);
				   void blockLB(LightBarrier);
				   void freeLB(LightBarrier);

				private:
				   HandlerDummy *mHandlerDummy;
				   RunManager *mRunManager;
				   config_t *mConfig;
			};

		}
	}
}

#endif /* SRC_TEST_UT_RUN_MANAGER_LOGIC_H */
