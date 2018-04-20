#ifdef ESEP_TEST

#include <iostream>
#include <exception>

#include "test/unit/manager.h"
#include "lib/utils.h"

namespace esep { namespace test { namespace unit {

Manager::results_t Manager::run(void)
{
	results_t results;

	auto onError = [&results](const TestSuite *s, const std::string& e) {
		results[s->name()].second = e;
	};

	std::cout << "Running unit test suites: " << std::flush;

	for(TestSuite *s : mTests)
	{
		try
		{
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

	std::cout << "\n" << std::flush;

	for(TestSuite *ts : mTests)
	{
		delete ts;
	}

	return results;
}

}}}

#endif
