#ifndef ESEP_TEST_UNIT_MANAGER_H
#define ESEP_TEST_UNIT_MANAGER_H

#include <ios>
#include <map>
#include <deque>

#include "lib/singleton.h"
#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Manager
			{
				typedef std::map<std::string, std::pair<TestSuite::result_t, std::string>> results_t;

				public:
					template<typename T>
						Manager& addTest( );
					results_t run( );
				private:
					std::deque<TestSuite *> mTests;
			};

			template<typename T>
			Manager& Manager::addTest(void)
			{
				mTests.push_front(new T);

				return *this;
			}
		}
	}
}

#endif

