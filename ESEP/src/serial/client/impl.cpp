#include "serial/client/impl.h"

#include "lib/logger.h"

namespace esep { namespace serial {

using namespace types;

BSPClient::Impl::Impl(BSPClient::connection_ptr c)
	: mBaseConnection(std::move(c))
	, mWriteConnection(mBaseConnection.get())
	, mReadConnection(mBaseConnection.get())
	, mReadWriteConnection(mReadConnection, mWriteConnection)
	, mSerializer(mWriteConnection)
	, mDeserializer(mReadConnection)
	, mWriter(mSerializer)
	, mReader(mWriter)
	, mReset(mReadWriteConnection)
{
	mRunning = true;

	mReaderThread.construct([this](void) {
		try
		{
			while(mRunning.load())
			{
				try
				{
					mReader.process(mDeserializer.receive());
				}
				catch(const types::BadPacketException& e)
				{
					mReset.handle();
					mWriter.reset();
					mReader.reset();
				}
				catch(const types::FailedPacketException& e)
				{
					mWriter.acknowledge(0, packet::Type::AP_ERR);
					mReader.reset();
				}
			}
		}
		catch(const types::storage_t::InterruptedException& e)
		{
		}
		catch(const Connection::ConnectionClosedException& e)
		{
		}
		MXT_CATCH_STRAY
	});
}

BSPClient::Impl::~Impl()
{
	mRunning = false;
	mBaseConnection->close();
}

void BSPClient::Impl::write(const types::buffer_t& data)
{
	mWriter.put(data);
}

buffer_t BSPClient::Impl::read(void)
{
	try
	{
		return mReader.get();
	}
	catch(const types::storage_t::InterruptedException& e)
	{
		MXT_THROW_EX(Connection::ConnectionClosedException);
	}
}

}}
