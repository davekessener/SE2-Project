#include "test/unit/assertions.h"

#include "test/ut/logger_format_parser.h"

namespace esep { namespace test { namespace ut {

typedef log::format::Parser Parser;

const char * const TEST_VARIABLES[] = {
	"date",
	"time",
	"line",
	"exception",
	"a"
};

LoggerFormatParser::LoggerFormatParser()
	: TestSuite("log/format::parser"), uut(nullptr)
{
}

void LoggerFormatParser::setup(void)
{
	uut = new Parser(TEST_VARIABLES);
}

void LoggerFormatParser::teardown(void)
{
	delete uut;
	uut = nullptr;
}

void LoggerFormatParser::define(void)
{
	UNIT_TEST("is empty at first")
	{
		ASSERT_EQUALS(uut->format.size(), 0u);
		ASSERT_EQUALS(uut->literals.size(), 0u);
	};

	UNIT_TEST("can find one ID")
	{
		uut->parseAll("%line%");

		ASSERT_EQUALS(uut->format.size(), 1u);
		ASSERT_EQUALS(uut->literals.size(), 0u);
		ASSERT_EQUALS(uut->format[0].first, 2);
		ASSERT_EQUALS(uut->format[0].second, 0u);
	};

	UNIT_TEST("can correctly parse literal offset")
	{
		uut->parseAll("%13line%");

		ASSERT_EQUALS(uut->format[0].second, 13u);
	};

	UNIT_TEST("can parse a long format string")
	{
		decltype(uut->format) testFormat({
			std::make_pair(-1, 0u),
			std::make_pair( 0, 0u),
			std::make_pair(-1, 0u),
			std::make_pair( 1, 21u),
			std::make_pair(-1, 0u),
			std::make_pair( 3, 0u),
			std::make_pair( 4, 0u),
			std::make_pair(-1, 0u)
		});
		decltype(uut->literals) testLiterals({
			"[",
			" ",
			"]: ",
			"!"
		});
		uut->parseAll("[%date% %21time%]: %exception%%a%!");

		ASSERT_EACH_EQUALS(uut->format, testFormat);
		ASSERT_EACH_EQUALS(uut->literals, testLiterals);
	};

	UNIT_TEST("fails an unterminated id")
	{
		ASSERT_ANY_FAILURE(uut->parseAll("%line"));
	};

	UNIT_TEST("fails an unknown id")
	{
		ASSERT_ANY_FAILURE(uut->parseAll("%something%"));
	};
}

}}}
