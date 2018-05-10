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
				Connection( ) : mID(INVALID_ID), mRef(nullptr) { }
				Connection(const Connection&);
				~Connection( );

				Connection& operator=(const Connection& c);
				void swap(Connection&) noexcept;

				void sendPulse(const pulse_t&);
				template<typename T>
					void sendPulse(const T& c, uint32_t v = 0)
						{ sendPulse<T, uint32_t>(c, v); }
				template<typename T1, typename T2>
					void sendPulse(const T1& c, const T2& v)
						{ sendPulse(pulse_t(c, v)); }

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
