
#ifndef SRC_TEST_UT_RUN_MANAGER_LOGIC_H
#define SRC_TEST_UT_RUN_MANAGER_LOGIC_H

#include <vector>

#include "test/unit/unit.h"
#include "base/run_manager.h"
#include "communication/packet.h"




namespace esep
{
	namespace test
	{
		namespace ut
		{

			struct BasicRecipient;

			class RunManagerLogic : public TestSuite
			{
				typedef base::RunManager::Event Event;
				typedef base::ConfigObject config_t;
				typedef base::RunManager RunManager;
				typedef communication::Packet Packet;
				typedef communication::Packet::Location Location;
				typedef communication::Message Message;
				typedef communication::Packet::msg_t msg_t;
				typedef communication::Packet_ptr Packet_ptr;
				typedef communication::Message::Run runMessage_t;
				typedef data::RunManagerTimer::TimerEvent TimerEvent;


				public:
				   RunManagerLogic();

				protected:
				   void setup( );
				   void teardown( );
				   void define( );

				private:
				   void sendPacket(msg_t);

				private:
				   BasicRecipient *mCom;
				   RunManager *mRunManager;
				   config_t *mConfig;
			};

		}
	}
}

#endif /* SRC_TEST_UT_RUN_MANAGER_LOGIC_H */
