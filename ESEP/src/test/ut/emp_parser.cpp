#include "test/ut/emp_parser.h"

#include "test/unit/assertions.h"

#include "emp/parser.h"

namespace esep { namespace test { namespace ut {

EMPParser::EMPParser(void)
	: TestSuite("EMP Parser")
{
}

void EMPParser::define(void)
{
	UNIT_TEST("can parse time")
	{
		ASSERT_EQUALS(emp::Parser::parseTime("00000000"), 0u);
		ASSERT_EQUALS(emp::Parser::parseTime("00000099"), 99u);
		ASSERT_EQUALS(emp::Parser::parseTime("12345678"), 12345678ull);
	};

	UNIT_TEST("can parse hw")
	{
		ASSERT_TRUE(emp::Parser::parseSensor("lb_start") == emp::Location::byName("lb_start"));
		ASSERT_TRUE(emp::Parser::parseSensor("LB_START") == emp::Location::byName("lb_start"));
		ASSERT_TRUE(emp::Parser::parseSensor("lB_sTaRt") == emp::Location::byName("lb_start"));

		ASSERT_FAILURE(emp::Parser::parseSensor("lb_middle"), emp::Location::UnknownLocationException);
		ASSERT_FAILURE(emp::Parser::parseSensor(""), emp::Location::UnknownLocationException);
	};

	UNIT_TEST("can parse values")
	{
		ASSERT_EQUALS(emp::Parser::parseValue("1"), 1u);
		ASSERT_EQUALS(emp::Parser::parseValue("0"), 0u);
		ASSERT_EQUALS(emp::Parser::parseValue("t"), 1u);
		ASSERT_EQUALS(emp::Parser::parseValue("F"), 0u);
		ASSERT_EQUALS(emp::Parser::parseValue("tRuE"), 1u);
		ASSERT_EQUALS(emp::Parser::parseValue("false"), 0u);
		ASSERT_EQUALS(emp::Parser::parseValue("0110101011100111"), 0x6AE7u);
		ASSERT_EQUALS(emp::Parser::parseValue("12345"), 12345u);
		ASSERT_EQUALS(emp::Parser::parseValue("$12ab"), 0x12ABu);
		ASSERT_EQUALS(emp::Parser::parseValue("0x78FE"), 0x78FEu);
		ASSERT_EQUALS(emp::Parser::parseValue("$0"), 0u);
		ASSERT_EQUALS(emp::Parser::parseValue("0xF"), 0xfu);
		ASSERT_EQUALS(emp::Parser::parseValue("9"), 9u);

		ASSERT_FAILURE(emp::Parser::parseValue(""), emp::Parser::ParseException);
		ASSERT_FAILURE(emp::Parser::parseValue("01"), emp::Parser::ParseException);
		ASSERT_FAILURE(emp::Parser::parseValue("$10000"), emp::Parser::ParseException);
		ASSERT_FAILURE(emp::Parser::parseValue("65536"), emp::Parser::ParseException);
		ASSERT_FAILURE(emp::Parser::parseValue("9/"), emp::Parser::ParseException);
		ASSERT_FAILURE(emp::Parser::parseValue(" 9"), emp::Parser::ParseException);
	};
}

}}}
