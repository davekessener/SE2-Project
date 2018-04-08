#ifdef ESEP_TEST

#include "test/unit/fsm.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

FSM::FSM(void)
	: TestSuite("FiniteStateMachine")
{
}

void FSM::setup(void)
{
	lib::fsm::Generator<event_t> g;

	mFSM = new FSM<event_t>(g.generate());
}

void FSM::teardown(void)
{
	delete mFSM;
}

void FSM::define(void)
{
	UNIT_TEST("can construct fsm")
	{
		ASSERT_TRUE(mFSM);
	};
}

}}}

#endif
