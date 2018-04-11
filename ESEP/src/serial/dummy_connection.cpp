#include "serial/dummy_connection.h"

namespace esep { namespace serial {

void DummyConnection::write(const byte_t *p, size_t n)
{
	while(n--) mWrite(*p++);
}

void DummyConnection::read(byte_t *p, size_t n)
{
	while(n--) *p++ = mRead();
}

}}
