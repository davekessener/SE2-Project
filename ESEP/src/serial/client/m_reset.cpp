#include "serial/client/m_reset.h"

namespace esep { namespace serial { namespace modules {

void Resetter::send(void)
{
	// TODO

	MXT_THROW("Sending RESET signal!");
}

void Resetter::respond(void)
{
	// TODO

	MXT_THROW("Received RESET signal!");
}

}}}
