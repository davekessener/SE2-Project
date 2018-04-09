#ifdef ESEP_TEST

#include <iostream>

#include "test/test.h"
#include "test/unit/manager.h"

#include "test/ut/logger_format_parser.h"
#include "test/ut/fsm.h"
#include "test/ut/byte_stream.h"

namespace esep { namespace test {

void runUnitTests(std::ostream& os)
{
	unit::Manager::instance()
		.addTest<unit::LoggerFormatParser>()
		.addTest<unit::FSM>()
		.addTest<unit::ByteStream>()
		.run(os);
}

void main(const std::vector<std::string>& args)
{
	runUnitTests(std::cout);
}

}}

#endif
