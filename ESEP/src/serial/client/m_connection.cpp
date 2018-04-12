#include <sstream>

#include "serial/client/m_connection.h"

#include "lib/logger.h"

#define MXT_N 18
#define MXT_MIN(a,b) (((a)<(b))?(a):(b))

namespace esep { namespace serial { namespace modules {

void Out_Connection::write(const types::buffer_t& o)
{
	get()([&o](Connection *c) {
		std::stringstream ss;

		for(uint i = 0, n = MXT_MIN(MXT_N, o.size()) ; i < n ; ++i)
		{
			ss << lib::hex<8>((&*o.cbegin())[i]); if(i + 1 != n) ss << " ";
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

		for(uint i = 0, n = MXT_MIN(MXT_N, b.size()) ; i < n ; ++i)
		{
			ss << lib::hex<8>(b[i]); if(i + 1 != n) ss << " ";
		}

		MXT_LOG(lib::stringify("Receiving packet (", b.size(), ") [", ss.str(), "]"));
	});

	o << lib::byte_stream::insert_all(b);
}

}}}
