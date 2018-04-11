#include "serial/client.h"
#include "serial/client/impl.h"

namespace esep { namespace serial {

Client::Client(Connection& c)
	: pImpl(new Impl(c))
{
}

Client::~Client(void)
{
	delete pImpl;
}

void Client::write(const buffer_t& data)
{
	types::buffer_t buf;

	for(const auto& e : data) buf.insert(e);

	pImpl->write(buf);
}

Client::buffer_t Client::read(void)
{
	types::buffer_t v = pImpl->read();
	buffer_t buf(v.cbegin(), v.cend());

	return buf;
}

}}
