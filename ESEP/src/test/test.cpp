#ifdef ESEP_TEST

#include <iostream>
#include <sstream>

#include "lib/stream_intercept.h"

#include "test/test.h"
#include "test/unit/manager.h"

#include "test/ut/logger_format_parser.h"
#include "test/ut/crc32.h"
#include "test/ut/fsm.h"
#include "test/ut/byte_stream.h"
#include "test/ut/dummy_connection.h"
#include "test/ut/serial_client.h"
#include "test/ut/qnx_connections.h"
#include "test/ut/timer.h"
#include "test/ut/watchdog.h"
#include "test/ut/sync_container.h"

namespace esep { namespace test {

void runUnitTests(void)
{
	std::stringstream logger;

	auto *os = lib::Logger::instance().setEcho(&logger);

	auto r = unit::Manager::instance()
		.addTest<unit::CRC32>()
		.addTest<unit::LoggerFormatParser>()
		.addTest<unit::FSM>()
		.addTest<unit::ByteStream>()
		.addTest<unit::QNXConnections>()
		.addTest<unit::SyncContainer>()
		.addTest<unit::Timer>()
		.addTest<unit::DummyConnection>()
		.addTest<unit::SerialClient>()
		.addTest<unit::Watchdog>()
		.run();

	uint w = 0;

	for(const auto& t : r)
	{
		if(t.first.size() > w) w = t.first.size();
	}

	for(const auto& t : r)
	{
		uint succ = 0, total = t.second.first.size();

		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::SUCCESS)
			{
				++succ;
			}
		}

		uint p = succ * 100 / total;

		std::cout << std::setw(w) << t.first << ": " << std::setw(3) << p << "% [" << std::setw(2) << succ << " / "
				  << std::setw(2) << total << "] tests successful\n";
	}

	for(const auto& t : r)
	{
		if(!t.second.second.empty())
		{
			std::cout << t.first << " has encountered a critical error: " << t.second.second << "!\n";
		}
	}

	for(const auto& t : r)
	{
		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::FAILURE)
			{
				std::cout << "[ERR] " << t.first << ": " << p.second << "\n";
			}
		}
	}

	lib::Logger::instance().setEcho(os);

	std::cout << "\n" << logger.str() << std::flush;
}

void main(const lib::args_t& args)
{
	runUnitTests();

	std::cout << "\nGoodbye." << std::endl;
}

}}

#endif
