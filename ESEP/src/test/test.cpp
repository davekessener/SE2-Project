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

namespace esep { namespace test {

void runUnitTests(std::ostream& os)
{
	unit::Manager::instance()
		.addTest<unit::LoggerFormatParser>()
		.addTest<unit::CRC32>()
		.addTest<unit::FSM>()
		.addTest<unit::ByteStream>()
		.addTest<unit::DummyConnection>()
		.addTest<unit::SerialClient>()
		.run(os);
}

void main(const std::vector<std::string>& args)
{
	std::stringstream my_cout, unit_testresults;

	{
		lib::StreamIntercept si(my_cout, std::cout);

		runUnitTests(unit_testresults);
	}

	std::cout << my_cout.str() << "\n" << unit_testresults.str();
}

}}

#endif
