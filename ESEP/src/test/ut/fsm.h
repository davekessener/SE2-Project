#ifndef ESEP_TEST_UNIT_FSM_H
#define ESEP_TEST_UNIT_FSM_H

#include <vector>

#include "lib/utils.h"
#include "lib/fsm.h"

#include "test/unit/unit.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class FSM : public TestSuite
			{
				enum class Event
				{
					I2A,
					A2B,
					A2C,
					B2A,
					C2B
				};

				typedef lib::fsm::FSM<Event> fsm_t;
				typedef lib::fsm::Generator<Event> gen_t;

				public:
					FSM( );
				protected:
					void setup( );
					void teardown( );
					void define( );
				private:
					fsm_t *mFSM;
					std::vector<std::string> mLog;
			};
		}
	}
}

#endif
