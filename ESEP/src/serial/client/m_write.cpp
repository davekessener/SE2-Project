#include "serial/client/m_write.h"

#include "lib/logger.h"

namespace esep { namespace serial { namespace modules {

class Writer::Impl
{
	public:
		Impl(Serializer& c, uint t) : mNextID(0), mTimeout(t), mConnection(c) { }
		void put(const types::buffer_t&);
		void acknowledge(types::id_t, packet::Type);
		void reset( );
		void send(packet::packet_ptr);
	private:
		void processNext( );
		void enqueue(packet::packet_ptr);
		void sendAndCheck(packet::packet_ptr);
	private:
		byte_t mNextID;
		const uint mTimeout;
		Serializer& mConnection;
		std::deque<packet::packet_ptr> mSendingBuffer;
		timer_t mTimer;
};

// # --------------------------------------------------------------------------------------------------

Writer::Writer(Serializer& c, uint t)
	: pImpl(new Impl(c, t))
{
}

Writer::~Writer(void)
{
	lock_t lock(mMutex);

	delete pImpl;
}

void Writer::put(const types::buffer_t& o)
{
	lock_t lock(mMutex);

	pImpl->put(o);
}

void Writer::acknowledge(types::id_t id, packet::Type t)
{
	lock_t lock(mMutex);

	pImpl->acknowledge(id, t);
}

void Writer::reset( )
{
	lock_t lock(mMutex);

	pImpl->reset();
}

void Writer::send(packet::packet_ptr p)
{
	lock_t lock(mMutex);

	pImpl->send(p);
}

// # --------------------------------------------------------------------------------------------------

namespace
{
	template<typename T>
	packet::packet_ptr createPacket(byte_t id, const types::buffer_t& o, size_t i, bool chained)
	{
#define MIN(a,b) (((a)<(b))?(a):(b))
		return packet::packet_ptr(new T(id, o.section(i, MIN(i + T::SIZE, o.size())), chained));
#undef MIN
	}
}

void Writer::Impl::put(const types::buffer_t& o)
{
	typedef packet::Data<packet::Type::SDP> sdp_t;
	typedef packet::Data<packet::Type::MDP> mdp_t;
	typedef packet::Data<packet::Type::LDP> ldp_t;

	size_t i = 0, s = o.size();

	if(!s) return;

	while(s > ldp_t::SIZE)
	{
		enqueue(createPacket<ldp_t>(mNextID++, o, i, true));

		i += ldp_t::SIZE;
		s -= ldp_t::SIZE;
	}

	if(s > mdp_t::SIZE)
	{
		enqueue(createPacket<ldp_t>(mNextID++, o, i, false));
	}
	else if(s > sdp_t::SIZE)
	{
		enqueue(createPacket<mdp_t>(mNextID++, o, i, false));
	}
	else
	{
		enqueue(createPacket<sdp_t>(mNextID++, o, i, false));
	}
}

void Writer::Impl::acknowledge(types::id_t id, packet::Type t)
{
	mTimer.reset();

	if(t == packet::Type::AP_OK)
	{
		if(!mSendingBuffer.empty() && id == mSendingBuffer.front()->getID())
		{
			processNext();
		}
	}
	else if(t == packet::Type::AP_ERR)
	{
		if(!mSendingBuffer.empty())
		{
			sendAndCheck(mSendingBuffer.front());
		}
	}
	else
	{
		MXT_THROW("Tried to pass a packet type (", id, ") that is not an AP");
	}
}

void Writer::Impl::reset( )
{
	if(!mSendingBuffer.empty())
	{
		sendAndCheck(mSendingBuffer.front());
	}
}

void Writer::Impl::send(packet::packet_ptr p)
{
	mConnection.send(p);
}

void Writer::Impl::processNext(void)
{
	mSendingBuffer.pop_front();

	if(!mSendingBuffer.empty())
	{
		sendAndCheck(mSendingBuffer.front());
	}
}

void Writer::Impl::enqueue(packet::packet_ptr p)
{
	if(mSendingBuffer.empty())
	{
		sendAndCheck(p);
	}

	mSendingBuffer.push_back(p);
}

void Writer::Impl::sendAndCheck(packet::packet_ptr p)
{
	send(p);

	mTimer = lib::Timer::instance().registerCallback([this](void) {
		try
		{
			acknowledge(0, packet::Type::AP_ERR);
		}
		catch(const Connection::ConnectionClosedException& e)
		{
		}
	}, mTimeout);
}

}}}
