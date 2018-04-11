#include <sstream>

#include "serial/client/m_connection.h"

#include "lib/logger.h"

namespace esep { namespace serial { namespace modules {

void Out_Connection::write(const types::buffer_t& o)
{
	get()([&o](Connection *c) {
		std::stringstream ss;

		for(const auto& e : o)
		{
			ss << lib::hex<8>(e) << " ";
		}

		MXT_LOG(lib::stringify("Sending packet (", o.size(), ") [", ss.str(), "]"));

		c->write(&*o.cbegin(), o.size());
	});
}

void In_Connection::read(types::buffer_t& o, size_t n)
{
	std::vector<byte_t> b(n);

	get()([&b, n](Connection *c) {
		c->read(&b[0], n);

		std::stringstream ss;

		for(const auto& e : b)
		{
			ss << lib::hex<8>(e) << " ";
		}

		MXT_LOG(lib::stringify("Receiving packet (", b.size(), ") [", ss.str(), "]"));
	});

	o << lib::byte_stream::insert_all(b);
}

}}}
