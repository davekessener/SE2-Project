#include "serial/utils/void_connection.h"

namespace esep { namespace serial {

void VoidConnection::write(const byte_t *, size_t)
{
	if(!mOpen.load())
		throw Connection::ConnectionClosedException();
}

void VoidConnection::read(byte_t *, size_t)
{
	lock_t lock(mMutex);

	mCond.wait(lock, [this](void) { return !mOpen.load(); });

	throw Connection::ConnectionClosedException();
}

void VoidConnection::close( )
{
	mOpen = false;

	mCond.notify_all();
}

}}
