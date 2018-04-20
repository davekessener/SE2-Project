#ifndef NO_QNX

#include <sys/neutrino.h>

#include "qnx/connection.h"

namespace esep { namespace qnx {

Connection::Connection(channel_id_t id)
	: mID(-1)
	, mRef(nullptr)
{
	if((mID = ConnectAttach(0, 0, id, 0, 0)) == -1)
	{
		MXT_THROW("Failed to open connection on channel ", id);
	}

	mRef = new uint;
	*mRef = 1;
}

Connection::Connection(const Connection& c)
{
	mID = c.mID;
	mRef = c.mRef;

	++*mRef;
}

Connection::~Connection(void)
{
	if(isConnected() && !--*mRef)
	{
		delete mRef;

		ConnectDetach(mID);
	}
}

Connection& Connection::operator=(const Connection& c)
{
	Connection t(c);

	swap(t);

	return *this;
}

void Connection::swap(Connection& c)
{
	std::swap(mID, c.mID);
	std::swap(mRef, c.mRef);
}

void Connection::sendPulse(const pulse_t& p)
{
	if(!isConnected())
		MXT_THROW("Cannot send when not connected!");

	if(MsgSendPulse(mID, -1, p.code, p.value) == -1)
	{
		MXT_THROW("Failed to send pulse msg {code: ", (uint)p.code, ", value: ", p.value, "}");
	}
}

void Connection::close(void)
{
	Connection t;

	swap(t);
}

}}

#else

namespace esep { namespace qnx {

Connection::Connection(channel_id_t id)
{
	MXT_THROW("Shouldn't call this!");
}

Connection::Connection(const Connection& c)
{
	MXT_THROW("Shouldn't call this!");
}

Connection::~Connection(void)
{
}

Connection& Connection::operator=(const Connection& c)
{
	MXT_THROW("Shouldn't call this!");
}

void Connection::swap(Connection& c)
{
	MXT_THROW("Shouldn't call this!");
}

void Connection::sendPulse(const pulse_t& p)
{
	MXT_THROW("Shouldn't call this!");
}

void Connection::close(void)
{
	MXT_THROW("Shouldn't call this!");
}

}}

#endif
