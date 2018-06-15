#ifndef ESEP_TEST_FN_RUNMANAGER_TEST_H
#define ESEP_TEST_FN_RUNMANAGER_TEST_H

#include <atomic>
#include <memory>

#include "lib/utils.h"

#include "communication/IRecipient.h"

#include "base/config_object.h"
#include "base/handler.h"

#include "hal/hal.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace r
			{
				class Tester : private communication::IRecipient
				{
					typedef communication::Packet Packet;
					typedef communication::Packet_ptr Packet_ptr;
					typedef communication::Message Message;
					typedef Packet::Location Location;
					typedef Packet::msg_t msg_t;

					struct Counter
					{
						Counter(int v) : v(v) { }
						operator int( ) const { return v; }
						Counter& operator=(int v) { this->v = v; return *this; }
						Counter operator++(int) { Counter t(*this); ++*this; return t; }
						Counter operator--(int) { Counter t(*this); --*this; return t; }
						Counter& operator++( );
						Counter& operator--( );

						int v = 0;
					};

					public:
						Tester( );
						~Tester( );
						void run( );

					private:
						void accept(Packet_ptr) override;

						void send(msg_t);

					private:
						std::unique_ptr<hal::HAL> mHAL;
						base::ConfigObject mConfig;
						std::unique_ptr<base::Handler> mHandler;
						std::atomic<bool> mRunning, mKeep;
						Counter mItemCount;
				};
			}
		}
	}
}

#endif
