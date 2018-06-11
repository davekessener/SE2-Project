#include "test/ut/run_manager_time_ctrl.h"
#include "test/unit/assertions.h"

namespace esep { namespace test { namespace ut {

#define MXT_SLEEP(t)		std::this_thread::sleep_for(std::chrono::milliseconds(t))

RunManagerTimeCtrl::RunManagerTimeCtrl()
	:	TestSuite("Run Manager - Time Controller"),
		mTimeCtrl(nullptr)
{

}

void RunManagerTimeCtrl::setup()
{
	mTimeCtrl = new TimeCtrl([this](TimerEvent e){
		mReceiveQ.push_back(e);
	});
	mReceiveQ.clear();
}

void RunManagerTimeCtrl::teardown()
{
	delete mTimeCtrl;
	mTimeCtrl = nullptr;
}

void RunManagerTimeCtrl::define()
{
	UNIT_TEST("can create a time controller")
	{
	};

	UNIT_TEST("register and receive one callback")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(8);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(2);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 1u);
		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::EXPECT_NEW);
	};

	UNIT_TEST("register many timer on different states")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 5);
		mTimeCtrl->setTimer(State::IN_LB_START, TimerEvent::START_1, 10);
		mTimeCtrl->setTimer(State::BW_START_HS, TimerEvent::HS_1, 15);
		mTimeCtrl->setTimer(State::BF_LB_HS, TimerEvent::SWITCH_1, 20);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(4);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(1);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 1u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 2u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 3u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 4u);

		MXT_SLEEP(10);
		ASSERT_EQUALS(mReceiveQ.size(), 4u);

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::EXPECT_NEW);
		mReceiveQ.pop_front();

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::START_1);
		mReceiveQ.pop_front();

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::HS_1);
		mReceiveQ.pop_front();

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::SWITCH_1);
		mReceiveQ.pop_front();
		ASSERT_EQUALS(mReceiveQ.size(), 0u);
	};

	UNIT_TEST("register many timer on same state")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 5);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::START_1, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::HS_1, 15);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::SWITCH_1, 20);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(4);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(1);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 1u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 2u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 3u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 4u);

		MXT_SLEEP(10);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 4u);

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::EXPECT_NEW);
		mReceiveQ.pop_front();

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::START_1);
		mReceiveQ.pop_front();

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::HS_1);
		mReceiveQ.pop_front();

		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::SWITCH_1);
		mReceiveQ.pop_front();
		ASSERT_EQUALS(mReceiveQ.size(), 0u);
	};

	UNIT_TEST("register many timer on same time")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(5);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 0u);
		MXT_SLEEP(10);
		ASSERT_APPROX_EQUALS(mReceiveQ.size(), 9u);
	};

	UNIT_TEST("pause and resume timer")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 15);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 15);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 15);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 15);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 15);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		MXT_SLEEP(5);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		mTimeCtrl->pauseAllTimer();
		MXT_SLEEP(10);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		mTimeCtrl->resumeAllTimer();
		MXT_SLEEP(5);
		ASSERT_EQUALS(mReceiveQ.size(), 5u);

		MXT_SLEEP(5);
		ASSERT_EQUALS(mReceiveQ.size(), 10u);
	};

	UNIT_TEST("delete timer")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::IN_LB_START, TimerEvent::HS_1, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::SWITCH_1, 20);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		mTimeCtrl->deleteTimer(State::BF_LB_START);
		MXT_SLEEP(15);
		ASSERT_EQUALS(mReceiveQ.size(), 1u);
		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::HS_1);
		mReceiveQ.pop_front();

		MXT_SLEEP(10);
		ASSERT_EQUALS(mReceiveQ.size(), 1u);
		ASSERT_EQUALS(mReceiveQ.front(), TimerEvent::SWITCH_1);
		mReceiveQ.pop_front();
	};

	UNIT_TEST("delete timer")
	{
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::EXPECT_NEW, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::HS_1, 10);
		mTimeCtrl->setTimer(State::BF_LB_START, TimerEvent::SWITCH_1, 20);
		ASSERT_EQUALS(mReceiveQ.size(), 0u);

		mTimeCtrl->deleteTimer(State::BF_LB_START);
		mTimeCtrl->deleteTimer(State::BF_LB_START);
		mTimeCtrl->deleteTimer(State::BF_LB_START);

	};

	UNIT_TEST("delete timer, negative test")
	{
		ASSERT_FAILURE(mTimeCtrl->deleteTimer(State::BF_LB_START), TimeCtrl::TimerAccessException);
	};
}





}}}



