#ifndef ESEP_TEST_UNIT_MANAGER_H
#define ESEP_TEST_UNIT_MANAGER_H

#include <ios>
#include <map>
#include <deque>

#include "lib/singleton.h"

#include "test/unit/unit.h"
#include "test/unit/hal.h"

#include "base/config_object.h"

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
					Manager( );
					~Manager( );
					template<typename T>
						Manager& addTest( );
					results_t run( );
				private:
					std::deque<TestSuite *> mTests;
					base::ConfigObject mConfig;
					TestHAL mHAL;
			};

			template<typename T>
			Manager& Manager::addTest(void)
			{
				T *suite = new T;

				suite->setHAL(&mHAL);

				mTests.push_front(suite);

				return *this;
			}
		}
	}
}

#endif

