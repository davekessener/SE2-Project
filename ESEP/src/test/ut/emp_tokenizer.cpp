#include "test/ut/emp_tokenizer.h"

#include "test/unit/assertions.h"

#include "emp/tokenizer.h"

namespace esep { namespace test { namespace unit {

EMPTokenizer::EMPTokenizer(void)
	: TestSuite("EMP Tokenizer")
{
}

void EMPTokenizer::define(void)
{
	UNIT_TEST("can process a line")
	{
		std::string time = "00000000", name = "lb_start", value = "0";
		std::string line = time + " " + name + " " + value;
		std::vector<std::string> buf, ref{time, name, value};

		emp::Tokenizer::tokenize(buf, line);

		ASSERT_EACH_EQUALS(buf, ref);
	};

	UNIT_TEST("can ignore comments")
	{
		std::string time = "00000000", name = "lb_start", value = "0";
		std::string line = time + " " + name + " " + value + " # a comment";
		std::vector<std::string> buf, ref{time, name, value};

		emp::Tokenizer::tokenize(buf, line);

		ASSERT_EACH_EQUALS(buf, ref);
	};

	UNIT_TEST("can parse tabs")
	{
		std::string time = "00000000", name = "lb_start", value = "0";
		std::string line = time + "    " + name + "\t" + value + " # a comment";
		std::vector<std::string> buf, ref{time, name, value};

		emp::Tokenizer::tokenize(buf, line);

		ASSERT_EACH_EQUALS(buf, ref);
	};

	UNIT_TEST("doesn't like newlines")
	{
		std::string time = "00000000", name = "lb_start", value = "0";
		std::string line = time + "\n   " + name + "\t" + value + " # a comment";
		std::vector<std::string> buf;

		ASSERT_FAILURE(emp::Tokenizer::tokenize(buf, line), emp::Tokenizer::MalformedLineException);
	};
}

}}}
