#ifdef ESEP_TEST

#include <iostream>
#include <sstream>

#include "lib/stream_intercept.h"

#include "test/test.h"
#include "test/unit/manager.h"

#include "test/ft/hal.h"

#include "test/ut/logger_format_parser.h"
#include "test/ut/crc32.h"
#include "test/ut/fsm.h"
#include "test/ut/byte_stream.h"
#include "test/ut/dummy_connection.h"
#include "test/ut/serial_client.h"
#include "test/ut/qnx_connections.h"

namespace esep { namespace test {

std::string runUnitTests(void)
{
	std::stringstream ss;

	auto r = unit::Manager::instance()
		.addTest<unit::LoggerFormatParser>()
		.addTest<unit::CRC32>()
		.addTest<unit::FSM>()
		.addTest<unit::ByteStream>()
		.addTest<unit::DummyConnection>()
		.addTest<unit::SerialClient>()
		.addTest<unit::QNXConnections>()
		.run();

	ss << "\nRunning automatic unit test suites:\n";

	for(const auto& t : r)
	{
		ss << t.first << ": ";

		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::SUCCESS)
			{
				ss << ".";
			}
			else
			{
				ss << "E";
			}
		}

		ss << "\n";
	}

	ss << "\n";

	for(const auto& t : r)
	{
		if(!t.second.second.empty())
		{
			ss << t.first << " has encountered a critical error: " << t.second.second << "!\n";
		}
	}

	for(const auto& t : r)
	{
		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::FAILURE)
			{
				ss << t.first << ": " << p.second << "\n";
			}
		}
	}

	return ss.str();
}

void main(const lib::args_t& args)
{
	std::cout << runUnitTests() << std::endl;

	{
		functional::HALTester hal_tester;

		hal_tester.run();
	}
}

}}

#endif
