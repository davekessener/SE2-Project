#ifndef ESEP_TEST_UNIT_MANAGER_H
#define ESEP_TEST_UNIT_MANAGER_H

#include <ios>
#include <map>

#include "lib/singleton.h"
#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class ManagerImpl
			{
				typedef std::map<std::string, std::pair<TestSuite::result_t, std::string>> results_t;

				public:
					template<typename T>
						ManagerImpl& addTest( );
					results_t run( );
				private:
					std::vector<TestSuite *> mTests;
			};

			typedef lib::SingletonHolder<ManagerImpl> Manager;

			template<typename T>
			ManagerImpl& ManagerImpl::addTest(void)
			{
				mTests.push_back(new T);

				return *this;
			}
		}
	}
}

#endif

