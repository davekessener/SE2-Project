#include "serial/dummy_connection.h"

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

void DummyConnection::write(const byte_t *p, size_t n)
{
	while(n--)
	{
		if(!isOpen())
			throw Connection::ConnectionClosedException();

		mBuffer.insert(*p++);
	}
}

void DummyConnection::read(byte_t *p, size_t n)
{
	while(n--)
	{
		if(!isOpen())
			throw Connection::ConnectionClosedException();

		byte_t v = mCounterpart->mBuffer.remove();

		if(!isOpen())
			throw Connection::ConnectionClosedException();

		*p++ = v;
	}
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
