#ifndef ESEP_TIMER_BASE_H
#define ESEP_TIMER_BASE_H

#include "lib/utils.h"
#include "lib/timer/types.h"

namespace esep
{
	namespace timer
	{
		class Base
		{
			public:
				Base(id_t id, uint r, uint p) : mID(id), mRemaining(r), mPeriod(p), mActive(true), mPaused(false){ }
				virtual ~Base( ) { }
				virtual void tick( );
				virtual void suspend() { mPaused = true; }
				virtual void resume() { mPaused = false; }
				virtual bool isPaused() const { return mPaused; }
				virtual void shutdown( ) { mActive = false; }
				virtual bool done( ) const { return !mActive; }

				id_t ID( ) const { return mID; }

			protected:
				virtual void execute( ) = 0;

			private:
				const id_t mID;
				uint mRemaining;
				const uint mPeriod;
				bool mActive;
				bool mPaused;
		};
	}
}

#endif
