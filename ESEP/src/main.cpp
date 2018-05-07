#include <iostream>

#ifdef ESEP_TEST
#	include "test/test.h"
#else
#	include "system.h"
#endif

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/timer.h"

#include "lib/io/network_writer.h"

namespace esep
{
	void init(void)
	{
		lib::Logger::instance();
		lib::Timer::instance();

		try
		{
//			lib::Logger::instance().setEcho(std::unique_ptr<lib::Writer>(new lib::NetworkWriter("127.0.0.1", 8000)));
		}
		catch(const lib::NetworkWriter::ConnectionFailedException& e)
		{
			MXT_LOG("Failed to connect to network logger. Echoing to stdout.");
		}
	}
}

int main(int argc, char *argv[])
try
{
	esep::lib::args_t args(argv + 1, argv + argc);

	esep::init();

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
