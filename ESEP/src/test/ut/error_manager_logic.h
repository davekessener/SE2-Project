
#ifndef TEST_UT_ERROR_MANAGER_LOGIC_H
#define TEST_UT_ERROR_MANAGER_LOGIC_H

#include "test/unit/unit.h"

#include "base/error_manager.h"
#include "hal/buttons.h"
#include "hal/light_barriers.h"
#include "data/location_data.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			struct BasicRecipient;

			class ErrorManagerLogic : public unit::TestSuite
			{
				typedef base::ErrorManager::Event Event;
				typedef hal::HAL::Field Field;
				typedef hal::Buttons::Button Button;
				typedef hal::LightBarriers::LightBarrier LightBarrier;
				typedef communication::Packet Packet;
				typedef communication::Packet::Location Location;
				typedef communication::Message Message;
				typedef communication::Packet::msg_t msg_t;
				typedef data::Location::Type dloc_t;

				public:
					ErrorManagerLogic( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					void send(Location, msg_t);
					void sendl(Location, msg_t, dloc_t);

				private:
					base::ErrorManager *mErrManager;
					BasicRecipient *mHandler;
			};
		}
	}
}



#endif
