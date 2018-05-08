
#ifdef ESEP_TEST

#include <iostream>

#include "test/test.h"

#include "test/ft/unit.h"
#include "test/ft/serial.h"
#include "test/ft/hal.h"
#include "test/ft/communication.h"

#include "lib/logger.h"

namespace esep { namespace test {

bool main(const lib::args_t& args)
{
	bool ut = functional::runUnitTests(true);

	if(ut)
	{
//		functional::testSerialConnection();
//		functional::testHAL();
		functional::testCommunicationLayer(args);
	}

	MXT_LOG("\nGoodbye.");

	return ut;
}

}}

#endif
