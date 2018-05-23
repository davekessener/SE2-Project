#include "master/hsm/manager.h"

namespace esep { namespace master { namespace hsm {

void Manager::enter(void)
{
	com().send(Location::BASE, mMessage);
}

}}}
