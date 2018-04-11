#ifdef ESEP_TEST

#include <iostream>
#include <exception>

#include "test/unit/unit.h"

namespace esep { namespace test { namespace unit {

void TestSuite::doTest(result_fn f)
{
	mTests.clear();
	mErrors.clear();

	define();

	for(const auto& p : mTests)
	{
		setup();

		try
		{
			p.second();

			f(Result::SUCCESS);
		}
		catch(const std::exception& e)
		{
			mErrors.push_back(p.first + ": " + e.what());

			f(Result::FAILURE);
		}
		catch(const std::string& e)
		{
			mErrors.push_back(p.first + ": '" + e + "' [std::string]");

			f(Result::FAILURE);
		}
		catch(...)
		{
			mErrors.push_back(p.first + " [unknown exception]");

			f(Result::FAILURE);
		}

		teardown();
	}
}

}}}

#endif
