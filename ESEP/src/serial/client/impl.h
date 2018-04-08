#ifndef ESEP_SERIAL_CLIENT_IMPL_H
#define ESEP_SERIAL_CLIENT_IMPL_H

#include "serial/client.h"
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
		class Client::Impl
		{
			public:
				Impl(Connection&&);
				~Impl( );
				void write(const types::buffer_t&);
				types::buffer_t read( );

			private:
				Connection *mBaseConnection;
				modules::Out_Connection mWriteConnection;
				modules::In_Connection mReadConnection;
				modules::SeperatedConnection mReadWriteConnection;
				modules::Serializer mSerializer;
				modules::Deserializer mDeserializer;
				modules::Reader mReader;
				modules::Writer mWriter;
				modules::Resetter mReset;

				bool mRunning;
				std::thread mWriterThread, mReaderThread;
		};
	}
}

#endif
