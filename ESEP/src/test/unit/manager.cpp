#ifdef ESEP_TEST

#include <iostream>
#include <exception>

#include "test/unit/manager.h"
#include "lib/utils.h"

namespace esep { namespace test { namespace unit {

void ManagerImpl::run(std::ostream& os)
{
	auto printStatus = [&os](TestSuite::Result r) {
		os << (r == TestSuite::Result::SUCCESS ? "." : "E");
	};

	auto onError = [&os](const TestSuite *s, const std::string& e) {
		os << "\nSuite '" << s->name() << "' has encountered a critical error:\n" << e << "\n";
	};

	os << "Running automatic unit test suites.\n";

	for(TestSuite *s : mTests)
	{
		try
		{
			os << s->name() << ": ";

			s->doTest(printStatus);

			os << "\n";
		}
		catch(const std::exception& e)
		{
			onError(s, lib::stringify(":\n", e.what(), "\n"));
		}
		catch(const std::string& e)
		{
			onError(s, lib::stringify(":\n", e, "\n"));
		}
		catch(...)
		{
			onError(s, lib::stringify(" of unknown type!\n"));
		}
	}

	for(TestSuite *s : mTests)
	{
		for(const std::string& e : s->errors())
		{
			os << "[" << s->name() << "] " << e << "\n";
		}
	}

	for(TestSuite *ts : mTests)
	{
		delete ts;
	}
}

}}}

#endif
