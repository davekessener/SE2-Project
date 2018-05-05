
#ifdef ESEP_TEST

#include <iostream>

#include "test/test.h"

#include "test/ft/unit.h"
#include "test/ft/serial.h"
#include "test/ft/hal.h"

namespace esep { namespace test {

bool main(const lib::args_t& args)
{
	bool ut = functional::runUnitTests(true);

	if(ut)
	{
//		functional::testSerialConnection();
//		functional::testHAL();
	}

	std::cout << "\nGoodbye." << std::endl;

	return ut;
}

}}

#endif
