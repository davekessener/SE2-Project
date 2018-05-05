#include <iostream>

#ifdef ESEP_TEST
#	include "test/test.h"
#else
#	include "system.h"
#endif

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/timer.h"

int main(int argc, char *argv[])
try
{
	esep::lib::args_t args(argv, argv + argc);

	esep::lib::Logger::instance();
	esep::lib::Timer::instance().reset();

#ifdef ESEP_TEST
	esep::test::main(args);
#else
	esep::System::instance().run(args);
#endif

	return 0;
}
catch(const std::string& e)
{
	std::cerr << "ERROR: Uncaught string! " << e << std::endl;
}
catch(const std::exception& e)
{
	std::cerr << "ERROR: Uncaught exception! " << e.what() << std::endl;
}
