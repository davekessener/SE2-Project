#include "serial/client/m_deserialize.h"

namespace esep { namespace serial { namespace modules {

packet::packet_ptr Deserialize::receive(void)
{
	return factory_t::produce(mConnection.read<Type>(), mConnection);
}

}}}
