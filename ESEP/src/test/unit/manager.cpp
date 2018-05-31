#include <iostream>
#include <exception>

#include "test/unit/manager.h"

#include "lib/utils.h"
#include "lib/logger.h"

#include "system.h"

namespace esep { namespace test { namespace unit {

Manager::results_t Manager::run(void)
{
	results_t results;

	auto onError = [&results](const TestSuite *s, const std::string& e) {
		results[s->name()].second = e;
		MXT_LOG_ERROR("Encountered unexpected exception: ", e);
	};

	HAL_CONSOLE.print("Running unit test suites: ");
	MXT_LOG("Running unit tests ...");

	for(TestSuite *s : mTests)
	{
		try
		{
			MXT_LOG("Running UT suite '", s->name(), "'");

			s->doTest();

			results[s->name()].first = s->results();
		}
		catch(const std::exception& e)
		{
			onError(s, lib::stringify("std::exception(", e.what(), ")"));
		}
		catch(const std::string& e)
		{
			onError(s, lib::stringify("std::string(", e, ")"));
		}
		catch(...)
		{
			onError(s, "[unknown error]");
		}
	}

	HAL_CONSOLE.println();

	for(TestSuite *ts : mTests)
	{
		delete ts;
	}

	return results;
}

}}}
