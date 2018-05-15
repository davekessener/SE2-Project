#ifndef ESEP_SERIAL_CLIENT_IMPL_H
#define ESEP_SERIAL_CLIENT_IMPL_H

#include <atomic>

#include "lib/thread.h"

#include "serial/bsp_client.h"
#include "serial/client/types.h"
#include "serial/client/m_connection.h"
#include "serial/client/m_serialize.h"
#include "serial/client/m_deserialize.h"
#include "serial/client/m_read.h"
#include "serial/client/m_write.h"
#include "serial/client/m_reset.h"

namespace esep
{
	namespace serial
	{
		class BSPClient::Impl
		{
			public:
				Impl(BSPClient::connection_ptr, uint);
				~Impl( );
				void write(const types::buffer_t&);
				types::buffer_t read( );
				size_t size( ) const { return mReader.size(); }

			private:
				BSPClient::connection_ptr mBaseConnection;
				modules::Out_Connection mWriteConnection;
				modules::In_Connection mReadConnection;
				modules::SeperatedConnection mReadWriteConnection;
				modules::Serializer mSerializer;
				modules::Deserializer mDeserializer;
				modules::Writer mWriter;
				modules::Reader mReader;
				modules::Resetter mReset;

				std::atomic<bool> mRunning;
				lib::Thread mReaderThread;
		};
	}
}

#endif
