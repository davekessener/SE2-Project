#include <iostream>

#include "system.h"

#ifdef ESEP_TEST
#	include "test/test.h"
#endif

int main(int argc, char *argv[])
try
{
#ifndef ESEP_TEST
	esep::System::instance().run();
#else
	esep::test::main(std::vector<std::string>(argv, argv + argc));
#endif

	return 0;
}
catch(const std::string& e)
{
	std::cerr << "ERROR: Uncaught exception!\n" << e << std::endl;
}
