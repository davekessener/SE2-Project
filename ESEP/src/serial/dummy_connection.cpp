#include "serial/dummy_connection.h"

#include "lib/logger.h"

#define MXT_N 16
#define MXT_MIN(a,b) (((a)<(b))?(a):(b))

namespace esep { namespace serial {

DummyConnection::DummyConnection(void)
	: mCounterpart(nullptr)
	, mBuffer(new buffer_t)
	, mTransform([](byte_t v, uint) { return v; })
	, mSentPackets(0)
{
}

DummyConnection::~DummyConnection(void)
{
	DummyConnection::close();
}

void DummyConnection::connect(DummyConnection& c)
{
	close();
	c.close();

	mCounterpart = &c;
	c.mCounterpart = this;
}

void DummyConnection::write(const byte_t * const p, const size_t n)
{
	if(!isOpen())
		MXT_THROW_EX(Connection::ConnectionClosedException);

	++mSentPackets;

	std::vector<byte_t> b(n);

	for(uint i = 0 ; i < n ; ++i)
	{
		b[i] = mTransform(p[i], i);
	}

//	std::stringstream ss;
//
//	for(int i = 0, l = MXT_MIN(MXT_N, n) ; i < l ; ++i)
//	{
//		ss << lib::hex<8>(b[i]); if(i + 1 != l) ss << " ";
//	}
//
//	if(n > MXT_N) ss << " ...";

	for(const auto& e : b)
	{
		mBuffer->insert(e);
	}

//	MXT_LOG(lib::stringify("Sent packet (", n, ") [",ss.str(), "]"));
}

void DummyConnection::read(byte_t *p, size_t n)
{
	if(!isOpen())
		MXT_THROW_EX(Connection::ConnectionClosedException);

//	auto l = n;
//	auto o = p;

	try
	{
		while(n--)
		{
			byte_t v = mCounterpart->mBuffer->remove();

			*p++ = v;
		}
	}
	catch(const buffer_t::InterruptedException& e)
	{
		MXT_THROW_EX(Connection::ConnectionClosedException);
	}

//	std::stringstream ss;
//
//	for(uint i = 0, ll = MXT_MIN(MXT_N, l) ; i < ll ; ++i)
//	{
//		ss << lib::hex<8>(o[i]); if(i + 1 != ll) ss << " ";
//	}
//
//	if(l > MXT_N)
//		ss << " ...";

//	MXT_LOG(lib::stringify("Received packet (", l, ") [", ss.str(), "]"));
}

void DummyConnection::close(void)
{
	if(isOpen())
	{
		DummyConnection *c = mCounterpart;
		mCounterpart = nullptr;

		c->close();

		mSentPackets = 0;
		mBuffer.reset(new buffer_t);
	}
}

}}
