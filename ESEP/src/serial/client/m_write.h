#ifndef ESEP_SERIAL_CLIENT_MODULE_WRITE_H
#define ESEP_SERIAL_CLIENT_MODULE_WRITE_H

#include "lib/utils.h"
#include "lib/fsm.h"

#include "serial/client/types.h"
#include "serial/client/m_serialize.h"

#include "serial/client/packet/answer_packet.h"

namespace esep
{
	namespace serial
	{
		namespace modules
		{
			class Writer
			{
				typedef std::vector<packet::packet_ptr> packets_t;
				typedef packets_t::const_iterator packet_iter;

				struct sent_packet
				{
					packet_ptr packet;
					bool ack;
				};

				enum class Event
				{
					PACKET_READY,
					ACK_OK,
					ACK_RETRANSMIT
				};

				public:
					Writer(Serializer& c) : mConnection(c) { }
					void put(const types::buffer_t& o);
					void acknowledge(const packet::Answer&);
					void reset( );
				private:
					Serializer& mConnection;
					std::deque<types::buffer_t> mWriteBuffer;
					packets_t mSendingBuffer;
					packet_iter mCurrent;
					lib::fsm::FSM<Event> mFSM;
			};
		}
	}
}

#endif
