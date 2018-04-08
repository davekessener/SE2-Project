#ifndef ESEP_TEST_UNIT_FSM_H
#define ESEP_TEST_UNIT_FSM_H

#include "test/unit/unit.h"

#include "lib/fsm.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class FSM : public TestSuite
			{
				typedef uint event_t;

				public:
					FSM( );
					void setup( );
					void teardown( );
					void define( );
				private:
					lib::fsm::FSM<event_t> *mFSM;
			};
		}
	}
}

#endif
