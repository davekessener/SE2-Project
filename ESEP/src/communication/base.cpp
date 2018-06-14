#include "communication/base.h"

#include "lib/indicator.h"
#include "lib/logger.h"

namespace esep { namespace communication {

typedef communication::Message Message;
typedef Packet::Location Location;

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
				try { while(mRunning.load())
				{
					lib::ByteStream bs(lib::byte_stream::from_container(mSerial->read()));

					auto s = bs.size();
					auto p = Packet::deserialize(bs);

					MXT_LOG_INFO("Received ", p, " [", s, "B]");

					mBuffer.insert(p);
				}}
				catch(const serial::Connection::ConnectionClosedException& e) { throw; }
				catch(const container_t::InterruptedException& e) { throw; }
				catch(...)
				{
					MXT_LOG_LAST_E;

					MXT_THROW_EX(serial::Connection::ConnectionClosedException);
				}
			}
			catch(const serial::Connection::ConnectionClosedException& e)
			{
				if(mRunning.load())
				{
					MXT_LOG_INFO("Com detected a dead connection: ", e.what());

					accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, Message::Error::SERIAL));
					accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Error::SERIAL));
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

void Base::setBase(IRecipient *b)
{
	if(mBase)
	{
		MXT_THROW_EX(BaseOverrideException);
	}

	mBase = b;
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

void Base::accept(Packet_ptr p)
{
	mBuffer.insert(p);
}

void Base::send(Packet_ptr p)
{
	if(mRunning.load()) try
	{
		lib::ByteStream bs;

		p->serialize(bs);

		MXT_LOG_INFO("Sending ", p, " [", bs.size(), "B]");

		mSerial->write(serial::Client::buffer_t(bs.cbegin(), bs.cend()));
	}
	catch(serial::Connection::ConnectionClosedException& e)
	{
	}
}

IRecipient& Base::base(void)
{
	if(!mBase)
	{
		MXT_THROW_EX(MissingBaseException);
	}

	return *mBase;
}

}}
