#ifndef ESEP_TIMER_ASYNC_H
#define ESEP_TIMER_ASYNC_H

#include <atomic>

#include "lib/timer/types.h"
#include "lib/timer/base.h"

#include "lib/utils.h"
#include "lib/thread.h"

#include "qnx/connection.h"

namespace esep
{
	namespace timer
	{
		class Async : public Base
		{
			public:
				Async(id_t, callback_fn, uint, uint);
				virtual ~Async( );
				void tick( ) override;
				void shutdown( ) override;
				bool done( ) const override { return !mIsActive.load(); }

			protected:
				void execute( ) override;

			private:
				callback_fn mCallback;
				std::atomic<bool> mRunning, mIsActive;
				qnx::Connection mConnection;
				lib::Thread mThread;
		};
	}
}

#endif
