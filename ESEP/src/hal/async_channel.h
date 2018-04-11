#ifndef SRC_HAL_ASYNC_CHANNEL_H
#define SRC_HAL_ASYNC_CHANNEL_H

#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		class AsyncChannel
		{
			public:
			typedef uint32_t message_t;
			typedef int connection_id_t;
			typedef int channel_id_t;

			public:
				AsyncChannel( );
				~AsyncChannel( );
				message_t getMessage( );
				connection_id_t getConnectionID( ) const { return mConnectionID; }
				channel_id_t getChannelID( ) const { return mChannelID; }
			private:
				connection_id_t mConnectionID;
				channel_id_t mChannelID;
		};
	}
}
#endif
