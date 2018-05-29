#include <iostream>
#include <exception>

#include "test/unit/unit.h"

#include "lib/utils.h"
#include "lib/logger.h"

#include "system.h"

namespace esep { namespace test { namespace unit {

void TestSuite::doTest(void)
{
	mTests.clear();
	mResults.clear();

	define();

	auto onFailure = [this](const std::string& s) {
		mResults.push_back(std::make_pair(Result::FAILURE, s));
		HAL_CONSOLE.print("E");
	};

	for(const auto& p : mTests)
	{
		setup();

		try
		{
			MXT_LOG("Executing UT '", p.first, "'");

			p.second();

			mResults.push_back(std::make_pair(Result::SUCCESS, ""));

			HAL_CONSOLE.print(".");
		}
		catch(const std::exception& e)
		{
			onFailure(lib::stringify(p.first, ": ", e.what()));
		}
		catch(const std::string& e)
		{
			onFailure(lib::stringify(p.first, ": '", e, "' [std::string]"));
		}
		catch(...)
		{
			onFailure(lib::stringify(p.first, " [unknown exception]"));
		}

		teardown();
	}
}

}}}
