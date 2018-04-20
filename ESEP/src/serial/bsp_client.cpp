#include "serial/client.h"
#include "serial/client/impl.h"

namespace esep { namespace serial {

BSPClient::BSPClient(connection_ptr c)
	: pImpl(new Impl(std::move(c)))
{
}

BSPClient::~BSPClient(void)
{
	delete pImpl; pImpl = nullptr;
}

void BSPClient::write(const buffer_t& data)
{
	types::buffer_t buf;

	for(const auto& e : data) buf.insert(e);

	pImpl->write(buf);
}

Client::buffer_t BSPClient::read(void)
{
	types::buffer_t v = pImpl->read();
	buffer_t buf(v.cbegin(), v.cend());

	return buf;
}

}}
