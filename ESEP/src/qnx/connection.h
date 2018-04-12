#ifndef ESEP_QNX_CONNECTION_H
#define ESEP_QNX_CONNECTION_H

#include "qnx/pulse.h"

namespace esep
{
	namespace qnx
	{
		class Connection
		{
			public:
			static constexpr connection_id_t INVALID_ID = -1;

			public:
				Connection( ) : mID(INVALID_ID), mRef(nullptr) { }
				Connection(const Connection&);
				~Connection( );
				Connection& operator=(const Connection& c);
				void swap(Connection&);
				void sendPulse(const pulse_t&);
				bool isConnected( ) const { return mID != INVALID_ID; }
				void close( );
			private:
				connection_id_t mID;
				uint *mRef;

			private:
				Connection(channel_id_t);

				friend class Channel;
		};
	}
}

#endif
