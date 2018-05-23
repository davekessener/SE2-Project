#ifndef ESEP_TEST_UNIT_MASTERLOGIC_H
#define ESEP_TEST_UNIT_MASTERLOGIC_H

#include "test/unit/unit.h"

#include "master/master.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			struct BasicRecipient;

			class MasterLogic : public TestSuite
			{
				public:
					MasterLogic( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					BasicRecipient *mCom;
					master::Master *mMaster;
			};
		}
	}
}

#endif
