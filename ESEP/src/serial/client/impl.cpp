#include "serial/client/impl.h"

namespace esep { namespace serial {

using namespace types;

Client::Impl::Impl(Connection& c)
	: mBaseConnection(c)
	, mWriteConnection(&mBaseConnection)
	, mReadConnection(&mBaseConnection)
	, mReadWriteConnection(mReadConnection, mWriteConnection)
	, mSerializer(mWriteConnection)
	, mDeserializer(mReadConnection)
	, mWriter(mSerializer)
	, mReader(mWriter)
	, mReset(mReadWriteConnection)
{
	mRunning = true;

	mReaderThread = std::thread([this](void) {
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
		catch(const Connection::ConnectionClosedException& e)
		{
		}
		catch(const std::exception& e)
		{
			MXT_LOG(lib::stringify("Caught a stray exception: ", e.what()));
		}
		catch(const std::string& e)
		{
			MXT_LOG(lib::stringify("Caught a stray std::string: ", e));
		}
	});
}

Client::Impl::~Impl()
{
	mRunning = false;
	mBaseConnection.close();
	mReaderThread.join();
}

void Client::Impl::write(const types::buffer_t& data)
{
	mWriter.put(data);
}

buffer_t Client::Impl::read(void)
{
	try
	{
		return mReader.get();
	}
	catch(const types::storage_t::InterruptedException& e)
	{
		MXT_THROW_EX(Client::ClosingException);
	}
}

}}
