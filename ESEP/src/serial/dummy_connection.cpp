#include "serial/dummy_connection.h"

namespace esep { namespace serial {

void DummyConnection::write(const byte_t *p, size_t n)
{
	if(!isOpen())
		MXT_THROW("Connection is not open!");

	while(n--) mBuffer.insert(*p++);
}

void DummyConnection::read(byte_t *p, size_t n)
{
	if(!isOpen())
		MXT_THROW("Connection is not open!");

	while(n--) *p++ = mCounterpart->mBuffer.remove();
}

}}
