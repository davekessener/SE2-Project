#include "master/hsm/error.h"

namespace esep { namespace master { namespace hsm {

void Error::handle(Packet_ptr p)
{
	mCom->send(Location::BASE, p->message());
}

}}}
