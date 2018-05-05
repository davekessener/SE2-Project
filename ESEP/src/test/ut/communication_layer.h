#ifndef ESEP_TEST_UT_COMMUNICATIONLAYER_H
#define ESEP_TEST_UT_COMMUNICATIONLAYER_H

#include <memory>
#include <vector>

#include "serial/utils/dummy_connection.h"

#include "communication/base.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class CommunicationLayer : public TestSuite
			{
				typedef  communication::Packet::Location Location;
				typedef  communication::Packet::Message Message;

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
					communication::Packet_ptr packet(Location s, Location t, Message m)
						{ return std::make_shared<communication::Packet>(s, t, m); }
					void wait( ) { while((mMasterCom && mMasterCom->busy()) || (mSlaveCom && mSlaveCom->busy())) lib::Timer::instance().sleep(1); }

				private:
					SimpleRecipient *mMaster, *mBaseM, *mBaseS;
					serial::DummyConnection *mConnections[2];
					communication::Base *mMasterCom, *mSlaveCom;
			};
		}
	}
}

#endif
