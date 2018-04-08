#include "serial/client/impl.h"

namespace esep { namespace serial {

using namespace types;

Client::Impl::Impl(Connection&& c)
	: mBaseConnection(new Connection(std::move(c)))
	, mWriteConnection(mBaseConnection)
	, mReadConnection(mBaseConnection)
	, mReadWriteConnection(mReadConnection, mWriteConnection)
	, mSerializer(mWriteConnection)
	, mDeserializer(mReadConnection)
	, mReader(mSerializer)
	, mWriter(mSerializer)
	, mReset(mReadWriteConnection)
{
	mRunning = true;

	mReadThread = std::thread([this](void) {
		try
		{
			while(mRunning)
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
