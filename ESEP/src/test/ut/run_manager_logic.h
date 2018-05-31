
#ifndef SRC_TEST_UT_RUN_MANAGER_LOGIC_H
#define SRC_TEST_UT_RUN_MANAGER_LOGIC_H

#include "test/unit/unit.h"
#include "base/run_manager.h"


namespace esep
{
	namespace test
	{
		namespace unit
		{

			struct BasicRecipient;

			class RunManagerLogic : public TestSuite
			{
				typedef base::ConfigObject config_t;

				public:
				   RunManagerLogic();

				protected:
				   void setup( );
				   void teardown( );
				   void define( );

				private:
				   BasicRecipient *mCom;
				   base::RunManager *mRunManager;
				   config_t *mConfig;

			};

		}
	}
}

#endif /* SRC_TEST_UT_RUN_MANAGER_LOGIC_H */
