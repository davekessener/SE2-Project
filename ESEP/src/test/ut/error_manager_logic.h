
#ifndef TEST_UT_ERROR_MANAGER_LOGIC_H
#define TEST_UT_ERROR_MANAGER_LOGIC_H

#include "test/unit/unit.h"

#include "base/error_manager.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			struct BasicRecipient;

			class ErrorManagerLogic : public TestSuite
			{
				typedef base::ErrorManager::Event Event;
				typedef communication::Packet Packet;
				typedef communication::Packet::Location Location;
				typedef communication::Message Message;
				typedef communication::Packet::msg_t msg_t;

				public:
					ErrorManagerLogic( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					void send(Location, msg_t);

				private:
					base::ErrorManager *mErrManager;
					BasicRecipient *mHandler;
			};
		}
	}
}



#endif
