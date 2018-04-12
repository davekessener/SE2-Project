#ifdef ESEP_TEST

#include <iostream>
#include <exception>

#include "test/unit/unit.h"

#include "lib/utils.h"

namespace esep { namespace test { namespace unit {

void TestSuite::doTest(void)
{
	mTests.clear();
	mResults.clear();

	define();

	for(const auto& p : mTests)
	{
		setup();

		try
		{
			p.second();

			mResults.push_back(std::make_pair(Result::SUCCESS, ""));
		}
		catch(const std::exception& e)
		{
			mResults.push_back(std::make_pair(Result::FAILURE, lib::stringify(p.first, ": ", e.what())));
		}
		catch(const std::string& e)
		{
			mResults.push_back(std::make_pair(Result::FAILURE, lib::stringify(p.first, ": '", e, "' [std::string]")));
		}
		catch(...)
		{
			mResults.push_back(std::make_pair(Result::FAILURE, lib::stringify(p.first, " [unknown exception]")));
		}

		teardown();
	}
}

}}}

#endif
