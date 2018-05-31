#ifndef ESEP_TEST_UT_COMMUNICATIONLAYER_H
#define ESEP_TEST_UT_COMMUNICATIONLAYER_H

#include <memory>
#include <vector>

#include "serial/utils/dummy_connection.h"

#include "communication/master.h"
#include "communication/slave.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class CommunicationLayer : public unit::TestSuite
			{
				typedef communication::Packet Packet;
				typedef communication::Message Message;
				typedef Packet::Location Location;
				typedef Packet::msg_t msg_t;

				struct SimpleRecipient : public communication::IRecipient
				{
					void accept(communication::Packet_ptr p)
					{
						packets.push_back(p);
					}

					std::vector<communication::Packet_ptr> packets;
				};

				public:
					CommunicationLayer( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					communication::Packet_ptr packet(Location s, Location t, msg_t m)
						{ return std::make_shared<communication::Packet>(s, t, m); }
					void wait( ) { while((mMasterCom && mMasterCom->busy()) || (mSlaveCom && mSlaveCom->busy())) lib::Timer::instance().sleep(1); }

				private:
					SimpleRecipient *mMaster, *mBaseM, *mBaseS;
					serial::DummyConnection *mConnections[2];
					communication::Master *mMasterCom;
					communication::Slave *mSlaveCom;
			};
		}
	}
}

#endif
