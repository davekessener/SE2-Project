#include "serial/utils/void_connection.h"

#include "lib/logger.h"

namespace esep { namespace serial {

void VoidConnection::write(const byte_t *, size_t)
{
	if(!mOpen.load())
		throw Connection::ConnectionClosedException();
}

void VoidConnection::read(byte_t *, size_t)
{
	MXT_SYNCHRONIZE;

	mCond.wait(lock, [this](void) { return !mOpen.load(); });

	throw Connection::ConnectionClosedException();
}

void VoidConnection::close( )
{
	mOpen = false;

	mCond.notify_all();
}

}}
