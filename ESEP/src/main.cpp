#include <iostream>

#ifdef ESEP_TEST
#	include "test/test.h"
#else
#	include "system.h"
#endif

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/singleton.h"

#include "lib/writer.h"
#include "lib/io/file_writer.h"
#include "lib/io/stream_writer.h"
#include "lib/io/network_writer.h"

namespace esep { namespace lib {

#define MXT_DEFAULT_LOGGER_FN "log.txt"
#define MXT_DEFAULT_LOGGER_IP "192.168.127.1"
#define MXT_DEFAULT_LOGGER_PORT 8000

void init( )
{
	Logger::instance().addSink(Writer_ptr(new FileWriter(MXT_DEFAULT_LOGGER_FN)), log::Severity::INFO);
	Logger::instance().addSink(Writer_ptr(new StreamWriter(std::cout)), log::Severity::WARNING);

	try
	{
		Logger::instance().addSink(Writer_ptr(new NetworkWriter(MXT_DEFAULT_LOGGER_IP, MXT_DEFAULT_LOGGER_PORT)), log::Severity::INFO);
	}
	catch(const NetworkWriter::ConnectionFailedException& e)
	{
		MXT_LOG_WARN("Could not connect to network logger ", MXT_DEFAULT_LOGGER_IP, ":", MXT_DEFAULT_LOGGER_PORT);
	}
}

}}

int main(int argc, char *argv[])
try
{
	esep::lib::args_t args(argv + 1, argv + argc);

	esep::lib::init();

#ifdef ESEP_TEST
	esep::test::main(args);
#else
	esep::System::instance().run(args);
#endif

	esep::lib::ExitManager::execute();

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
