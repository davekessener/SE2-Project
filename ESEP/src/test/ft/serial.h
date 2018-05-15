#ifndef ESEP_FUNCTIONAL_SERIAL_H
#define ESEP_FUNCTIONAL_SERIAL_H

#include "lib/arguments.h"

#include "test/ft/serial/watchdog.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			void testSerialConnection(const lib::Arguments&) { testWatchdog(); }
		}
	}
}

#endif
