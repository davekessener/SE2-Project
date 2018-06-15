#ifndef ESEP_TEST_UT_MASTERLOGIC_H
#define ESEP_TEST_UT_MASTERLOGIC_H

#include "test/unit/unit.h"

#include "master/master.h"
#include "master/item.h"

#include "master/plugin/hausdorff.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class MasterLogic : public unit::TestSuite
			{
				struct BasicRecipient;

				typedef communication::Packet Packet;
				typedef communication::Message Message;
				typedef Packet::Location Location;
				typedef Packet::msg_t msg_t;
				typedef master::Plugin Plugin;
				typedef master::Plugin_ptr Plugin_ptr;
				typedef master::Item Item;

				public:
					MasterLogic( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					void send(Location, msg_t);

				private:
					master::plugin::Hausdorff::processor_t mProcessor;
					BasicRecipient *mCom;
					master::Master *mMaster;
					std::vector<Item> mItems;
			};
		}
	}
}

#endif
