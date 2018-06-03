#ifndef SRC_TEST_UT_RUN_MANAGER_TIME_CTRL_H
#define SRC_TEST_UT_RUN_MANAGER_TIME_CTRL_H

#include <functional>
#include <queue>

#include "test/unit/unit.h"
#include "base/run/time_controller.h"
#include "base/run/types.h"
#include "data/run_manager_timer_data.h"


namespace esep
{
	namespace test
	{
		namespace ut
		{

			class RunManagerTimeCtrl : public unit::TestSuite
			{
				typedef base::run::State State;
				typedef base::run::TimeCtrl TimeCtrl;
				typedef data::RunManagerTimer::TimerEvent TimerEvent;

				public:
					RunManagerTimeCtrl();


				protected:
				   void setup( ) override;
				   void teardown( ) override;
				   void define( ) override;

				private:
				   void takeCallback(TimerEvent);

				private:
				   TimeCtrl * mTimeCtrl;
				   std::deque<TimerEvent> mReceiveQ;
			};

		}
	}
}



#endif /* SRC_TEST_UT_RUN_MANAGER_TIME_CTRL_H */
