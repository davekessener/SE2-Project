#ifndef ESEP_SERIAL_DUMMY_CONNECTION_H
#define ESEP_SERIAL_DUMMY_CONNECTION_H

#include <functional>
#include <memory>
#include <atomic>

#include "serial/connection.h"

#include "lib/sync/container.h"
#include "lib/logger.h"

namespace esep
{
	namespace serial
	{
		class DummyConnection : public Connection
		{
			public:
			typedef sync::Container<byte_t> buffer_t;
			typedef std::function<byte_t(byte_t, uint)> transform_fn;

			public:
				DummyConnection( );
				~DummyConnection( );
				void connect(DummyConnection&);
				void write(const byte_t *, size_t);
				void read(byte_t *, size_t);
				void open(const std::string&) { MXT_THROW("Cannot open a dummy-connection!"); }
				void close( );
				bool isOpen( ) const { return mCounterpart; }
				void setTransform(transform_fn f) { mTransform = f; }
				uint getSentPackets( ) const { return mSentPackets; }
			private:
				DummyConnection *mCounterpart;
				std::unique_ptr<buffer_t> mBuffer;
				transform_fn mTransform;
				uint mSentPackets;
		};
	}
}

#endif
