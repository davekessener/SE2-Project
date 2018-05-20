#include "master/hsm/base.h"

namespace esep { namespace master {

void Base::handle(event_t e)
{
	MXT_THROW_EX(UnhandledEventException);
}

}}
