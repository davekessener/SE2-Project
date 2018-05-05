#ifndef ESEP_SERIAL_DUMMY_CONNECTION_H
#define ESEP_SERIAL_DUMMY_CONNECTION_H

#include <functional>
#include <memory>
#include <atomic>
#include <mutex>

#include "serial/connection.h"

#include "lib/sync/container.h"
#include "lib/logger.h"

namespace esep
{
	namespace serial
	{
		class DummyConnection : public Connection
		{
			typedef std::unique_lock<std::mutex> lock_t;

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
				bool isOpen( ) const { return mConnected.load(); }
				void setTransform(transform_fn f) { mTransform = f; }
				uint getSentPackets( ) const { return mSentPackets; }
				void kill( ) { mDead = true; }
			private:
				DummyConnection *mCounterpart;
				buffer_t mBuffer;
				transform_fn mTransform;
				uint mSentPackets;
				std::atomic<bool> mDead, mConnected;
				std::mutex mMutex;
		};
	}
}

#endif
