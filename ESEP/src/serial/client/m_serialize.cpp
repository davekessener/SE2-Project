#include "serial/client/m_serialize.h"

namespace esep { namespace serial { namespace modules {

void Serialize::send(packet::packet_ptr p)
{
	mConnection.write(p->serialize());
}

}}}
