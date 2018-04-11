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

	mReadThread = std::thread([this](void) {
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
					mReset.send();
				}
				catch(const types::ResetTriggeredException& e)
				{
					mReset.receive();
				}
				catch(const types::FailedPacketRead& e)
				{
				}
			}
		}
		catch(...)
		{
			if(mRunning)
			{
				throw;
			}
			else
			{
				MXT_LOG("Serial connection was terminated.");
			}
		}
	});
}

Client::Impl::~Impl()
{
	mRunning = false;
	delete mBaseConnection;
	mReadThread.join();
}

void Client::Impl::write(const buffer_t& data)
{
	mWriter.put(data);
}

buffer_t Client::Impl::read(void)
{
	return mReader.get();
}

}}
