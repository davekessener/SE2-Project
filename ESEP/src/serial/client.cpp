#include "serial/client.h"
#include "serial/client/impl.h"

namespace esep { namespace serial {

Client::Client(Connection&& c)
	: pImpl(new Impl(std::move(c)))
{
}

Client::~Client(void)
{
	delete pImpl;
}

void Client::write(const buffer_t& data)
{
	pImpl->write(data);
}

Client::buffer_t Client::read(void)
{
	return pImpl->read();
}

}}
