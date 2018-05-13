#include <iostream>

#include "test/test.h"

#include "system.h"

#include "lib/utils.h"
#include "lib/arguments.h"
#include "lib/logger.h"
#include "lib/singleton.h"

#include "lib/writer.h"
#include "lib/io/file_writer.h"
#include "lib/io/stream_writer.h"
#include "lib/io/network_writer.h"

namespace esep { namespace lib {

#define MXT_LOGGER_FN "log"
#define MXT_LOGGER_IP "logger-ip"
#define MXT_LOGGER_PORT "logger-port"
#define MXT_LOGGER_VERBOSE "verbose"

void init(const lib::Arguments& args)
{
	if(args.has(MXT_LOGGER_FN))
	{
		Logger::instance().addSink(Writer_ptr(new FileWriter(args.get(MXT_LOGGER_FN))), log::Severity::INFO);
	}

	if(args.has(MXT_LOGGER_VERBOSE))
	{
		Logger::instance().addSink(Writer_ptr(new StreamWriter(std::cout)), log::Severity::WARNING);
	}

	if(args.has(MXT_LOGGER_IP) && args.has(MXT_LOGGER_PORT))
	{
		std::string ip = args.get(MXT_LOGGER_IP);
		uint port = lexical_cast<uint>(args.get(MXT_LOGGER_PORT));

		try
		{
			Logger::instance().addSink(Writer_ptr(new NetworkWriter(ip, port)), log::Severity::INFO);
		}
		catch(const NetworkWriter::ConnectionFailedException& e)
		{
			MXT_LOG_WARN("Could not connect to network logger ", ip, ":", port);
		}
	}
}

}}

int main(int argc, char *argv[])
try
{
	esep::lib::Arguments args(argv + 1, argv + argc);

	esep::lib::init(args);

	if(esep::test::main(args) && args.has("run"))
	{
		esep::System::instance().run(args);
	}

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
