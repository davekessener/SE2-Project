#ifdef ESEP_TEST

#include <functional>

#include "test/ut/fsm.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

class MyState : public lib::fsm::State
{
	typedef std::function<void(const std::string&)> log_fn;

	public:
		MyState(const std::string& id, log_fn f) : mID(id), mLog(f) { }
		void enter( ) { mLog("entering " + mID); }
		void exit( ) { mLog("exiting " + mID); }
		const std::string& ID( ) const { return mID; }
	private:
		std::string mID;
		log_fn mLog;
};


FSM::FSM(void)
	: TestSuite("Finite State Machine")
	, mFSM(nullptr)
{
}

void FSM::setup(void)
{
	using lib::fsm::State;
	typedef State *State_ptr;

	gen_t fsmGen;

	fsm_t::transition_fn t = [this](State& s1, State& s2, Event e) {
		MyState& ms1(static_cast<MyState&>(s1)), ms2(static_cast<MyState&>(s2));

		mLog.push_back(lib::stringify(ms1.ID(), " -(", static_cast<uint>(e), ")> ", ms2.ID()));
	};
	auto f = [this](const std::string& s) {
		mLog.push_back(s);
	};

	State_ptr init = new MyState("I", f);
	State_ptr a = new MyState("A", f);
	State_ptr b = new MyState("B", f);
	State_ptr c = new MyState("C", f);

	mLog.clear();

	fsmGen.addStates(init, a, b, c);
	fsmGen.addTransition(init, a, Event::I2A, t);
	fsmGen.addTransition(a, b, Event::A2B, t);
	fsmGen.addTransition(a, c, Event::A2C, t);
	fsmGen.addTransition(b, a, Event::B2A, t);
	fsmGen.addTransition(c, b, Event::C2B, t);

	mFSM = new fsm_t(fsmGen.generate());
}

void FSM::teardown(void)
{
	delete mFSM;
}

void FSM::define(void)
{
	UNIT_TEST("can create a fsm")
	{
	};

	UNIT_TEST("can't create an empty fsm")
	{
		gen_t generator;

		ASSERT_ANY_FAILURE(generator.generate());
	};

	UNIT_TEST("can do one transition")
	{
		std::vector<std::string> c{"exiting I", "I -(0)> A", "entering A"};

		mFSM->process(Event::I2A);

		ASSERT_EACH_EQUALS(mLog, c);
	};

	UNIT_TEST("ignores unknown events")
	{
		mFSM->process(Event::A2B);

		ASSERT_EQUALS(mLog.size(), 0u);
	};

	UNIT_TEST("can process a sequence of events")
	{
		std::vector<std::string> c{
			"exiting I",
			"I -(0)> A",
			"entering A",
			"exiting A",
			"A -(2)> C",
			"entering C",
			"exiting C",
			"C -(4)> B",
			"entering B"
		};

		mFSM->process(Event::I2A);
		mFSM->process(Event::A2C);
		mFSM->process(Event::I2A);
		mFSM->process(Event::C2B);
		mFSM->process(Event::A2C);

		ASSERT_EACH_EQUALS(mLog, c);
	};
}

}}}

#endif
