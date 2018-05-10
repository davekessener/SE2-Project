#include "serial/utils/dummy_connection.h"

#include "lib/logger.h"

// #define LOG_DUMMY

#ifdef LOG_DUMMY
#	define MXT_N 16
#	define MXT_MIN(a,b) (((a)<(b))?(a):(b))
#endif

namespace esep { namespace serial {

DummyConnection::DummyConnection(void)
	: mCounterpart(nullptr)
	, mTransform([](byte_t v, uint) { return v; })
	, mSentPackets(0)
{
	mDead = false;
	mConnected = false;
}

DummyConnection::~DummyConnection(void)
{
	DummyConnection::close();
}

void DummyConnection::connect(DummyConnection& c)
{
	close();
	c.close();

	{
		MXT_SYNCHRONIZE;

		mCounterpart = &c;
		c.mCounterpart = this;
		mConnected = true;
		c.mConnected = true;
	}
}

void DummyConnection::write(const byte_t * const p, const size_t n)
{
	MXT_SYNCHRONIZE;

	if(!isOpen())
		MXT_THROW_EX(Connection::ConnectionClosedException);

	++mSentPackets;

	if(mDead.load())
		return;

#ifndef LOG_DUMMY
	for(uint i = 0 ; i < n ; ++i)
	{
		mBuffer.insert(mTransform(p[i], i));
	}
#else
	std::vector<byte_t> b(n);

	for(uint i = 0 ; i < n ; ++i)
	{
		b.at(i) = mTransform(p[i], i);
	}

	std::stringstream ss;

	for(int i = 0, l = MXT_MIN(MXT_N, n) ; i < l ; ++i)
	{
		ss << lib::hex<8>(b[i]); if(i + 1 != l) ss << " ";
	}

	if(n > MXT_N) ss << " ...";

	for(const auto& e : b)
	{
		mBuffer->insert(e);
	}

	MXT_LOG(lib::stringify("Sent packet (", n, ") [",ss.str(), "]"));
#endif
}

void DummyConnection::read(byte_t *p, size_t n)
{
	MXT_SYNCHRONIZE;

	if(!isOpen())
		MXT_THROW_EX(Connection::ConnectionClosedException);

#ifdef LOG_DUMMY
	auto l = n;
	auto o = p;
#endif

	try
	{
		while(n--)
		{
			if(!isOpen())
				MXT_THROW_EX(Connection::ConnectionClosedException);

			auto c = mCounterpart;

			lock.unlock();
			*(p++) = c->mBuffer.remove();
			lock.lock();
		}
	}
	catch(const buffer_t::InterruptedException& e)
	{
		MXT_THROW_EX(Connection::ConnectionClosedException);
	}

#ifdef LOG_DUMMY
	std::stringstream ss;

	for(uint i = 0, ll = MXT_MIN(MXT_N, l) ; i < ll ; ++i)
	{
		ss << lib::hex<8>(o[i]); if(i + 1 != ll) ss << " ";
	}

	if(l > MXT_N)
		ss << " ...";

	MXT_LOG(lib::stringify("Received packet (", l, ") [", ss.str(), "]"));
#endif
}

void DummyConnection::close(void)
{
	if(isOpen())
	{
		MXT_SYNCHRONIZE;

		mConnected = false;

		mCounterpart->close();

		mSentPackets = 0;
		mBuffer.interrupt();

		mCounterpart = nullptr;
	}
}

}}
