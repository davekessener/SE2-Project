#ifndef ESEP_TEST_FN_MASTER_SYSTEM_H
#define ESEP_TEST_FN_MASTER_SYSTEM_H

#include <array>
#include <vector>

#include "lib/writer.h"
#include "lib/reader.h"

#include "communication/packet.h"

#include "test/ft/master/station.h"

#include "master/master.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace m
			{
				class System : private communication::IRecipient
				{
					typedef std::vector<std::string> args_t;
					typedef communication::Packet Packet;
					typedef communication::Packet_ptr Packet_ptr;
					typedef communication::Message Message;
					typedef Packet::Location Location;

					enum
					{
						S_M1_START,
						S_M1_HS,
						S_M1_SWITCH,
						S_M1_END,
						S_M2_START,
						S_M2_HS,
						S_M2_SWITCH,
						S_M2_END,
						S_STATIONS
					};
					public:
						System(lib::Reader_ptr, lib::Writer_ptr);
						~System( );
						void run( );

					private:
						void flush( );

						void accept(Packet_ptr) override;

						void printHelp(const args_t&);
						void quit(const args_t&);
						void newItem(const args_t&);
						void printStatus(const args_t&);
						void switch2Run(const args_t&);
						void advance(const args_t&);
						void list(const args_t&);
						void remove(const args_t&);

					private:
						lib::Reader_ptr mIn;
						lib::Writer_ptr mOut;
						std::array<Station *, S_STATIONS> mStations;
						bool mRunning;
						master::Master mMaster;
						std::vector<Packet_ptr> mInBuf, mOutBuf;
						bool mBeltActive[2];
						Item::id_t mNextItemID;
						std::vector<Item_ptr> mOldItems;
						std::map<Item::id_t, int> mItems;
				};
			}
		}
	}
}

#endif
