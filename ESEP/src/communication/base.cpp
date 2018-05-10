#include "communication/base.h"

#include "lib/indicator.h"
#include "lib/logger.h"

namespace esep { namespace communication {

Base::Base(Client_ptr c)
	: mBase(nullptr)
	, mSerial(std::move(c))
{
	mRunning = false;
	mShuttingDown = false;

	mProcessingThread.construct([this](void) {
		try
		{
			lib::Indicator is_active(mRunning);

			while(true)
			{
				auto p = mBuffer.remove();

				if(!mRunning.load())
					break;

				process(p);
			}
		}
		catch(container_t::InterruptedException& e)
		{
		}
		MXT_CATCH_ALL_STRAY
	});

	mSerialThread.construct([this](void) {
		try
		{
			try
			{
				while(mRunning.load())
				{
					lib::ByteStream bs(lib::byte_stream::from_container(mSerial->read()));

					mBuffer.insert(Packet::deserialize(bs));
				}
			}
			catch(const serial::Connection::ConnectionClosedException& e)
			{
				if(mRunning.load())
				{
					accept(std::make_shared<Packet>(Packet::Location::MASTER, Packet::Location::BASE, Packet::Message::ERROR_SERIAL));
					accept(std::make_shared<Packet>(Packet::Location::BASE, Packet::Location::MASTER, Packet::Message::ERROR_SERIAL));
				}
			}
		}
		catch(container_t::InterruptedException& e)
		{
		}
		MXT_CATCH_ALL_STRAY
	});
}

Base::~Base(void)
{
	if(!mShuttingDown.load())
	{
		MXT_LOG("Communication layer not shut down correctly!");
	}
}

void Base::shutdown(void)
{
	mRunning = false;
	mBuffer.interrupt(true);
	mSerial.reset();
	mProcessingThread.join();
	mSerialThread.join();
	mShuttingDown = true;
}

void Base::setBase(IRecipient *p)
{
	if(mBase)
	{
		MXT_THROW_EX(BaseOverrideException);
	}

	mBase = p;
}

void Base::accept(Packet_ptr p)
{
	mBuffer.insert(p);
}

IRecipient& Base::base(void)
{
	if(!mBase)
	{
		MXT_THROW_EX(MissingBaseException);
	}

	return *mBase;
}

void Base::send(Packet_ptr p)
{
	if(mRunning.load()) try
	{
		lib::ByteStream bs;

		p->serialize(bs);

		mSerial->write(serial::Client::buffer_t(bs.cbegin(), bs.cend()));
	}
	catch(serial::Connection::ConnectionClosedException& e)
	{
	}
}

}}
