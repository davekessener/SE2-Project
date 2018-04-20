#include "serial/dummy_connection.h"

#include "lib/logger.h"

#define MXT_N 16
#define MXT_MIN(a,b) (((a)<(b))?(a):(b))

namespace esep { namespace serial {

void DummyConnection::connect(DummyConnection& c)
{
	close();
	c.close();

	mCounterpart = &c;
	c.mCounterpart = this;

	mBuffer.clear();
	c.mBuffer.clear();
}

void DummyConnection::write(const byte_t * const p, const size_t n)
{
//	MXT_LOG(lib::stringify("Buffer size is ", mBuffer.size(), ", inserting ", n, " bytes now."));

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
//
//	for(const auto& e : b)
//	{
//		mBuffer.insert(e);
//	}
//
//	MXT_LOG(lib::stringify("Sent packet (", n, ") [",ss.str(), "]"));
}

void DummyConnection::read(byte_t *p, size_t n)
{
//	MXT_LOG(lib::stringify("Other buffer size is ", mCounterpart->mBuffer.size(), ", removing ", n, " bytes now."));

//	auto l = n;
//	auto o = p;

	while(n--)
	{
		if(!isOpen())
			throw Connection::ConnectionClosedException();

		byte_t v = mCounterpart->mBuffer.remove();

		if(!isOpen())
			throw Connection::ConnectionClosedException();

		*p++ = v;
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
//
//	MXT_LOG(lib::stringify("Received packet (", l, ") [", ss.str(), "]"));
}

void DummyConnection::close(void)
{
	if(isOpen())
	{
		DummyConnection *c = mCounterpart;
		mCounterpart = nullptr;

		c->close();

		mBuffer.insert(0);
	}
}

}}
