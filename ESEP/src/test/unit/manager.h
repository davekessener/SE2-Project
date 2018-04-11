#ifndef ESEP_TEST_UNIT_MANAGER_H
#define ESEP_TEST_UNIT_MANAGER_H

#include <ios>

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
				public:
					template<typename T>
						ManagerImpl& addTest( );
					void run(std::ostream&);
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

