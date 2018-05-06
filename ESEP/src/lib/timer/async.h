#ifndef ESEP_TIMER_ASYNC_H
#define ESEP_TIMER_ASYNC_H

#include <atomic>

#include "lib/timer/types.h"

#include "lib/utils.h"
#include "lib/thread.h"

#include "qnx/connection.h"

namespace esep
{
	namespace timer
	{
		class Async
		{
			public:
				Async(id_t, callback_t, uint, uint);
				~Async( );
				void tick( );
				void shutdown( );
				id_t ID( ) const { return mID; }
				bool done( ) const { return !mIsActive.load(); }
			private:
				const id_t mID;
				callback_t mCallback;
				uint mRemainder;
				const uint mPeriod;
				std::atomic<bool> mRunning, mIsActive;
				qnx::Connection mConnection;
				lib::Thread mThread;
		};
	}
}

#endif
