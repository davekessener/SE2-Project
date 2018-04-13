#include <sstream>

#include "serial/client/m_connection.h"

#include "lib/logger.h"

namespace esep { namespace serial { namespace modules {

void Out_Connection::write(const types::buffer_t& o)
{
	get()([&o](Connection *c) {
		c->write(&*o.cbegin(), o.size());
	});
}

void In_Connection::read(types::buffer_t& o, size_t n)
{
	std::vector<byte_t> b(n);

	get()([&b, n](Connection *c) {
		c->read(&b[0], n);
	});

	o << lib::byte_stream::insert_all(b);
}

}}}
