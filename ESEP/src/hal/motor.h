#ifndef ESEP_HAL_MOTOR_H
#define ESEP_HAL_MOTOR_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Motor
		{
			public:
				Motor(HAL *);
				~Motor( );
				void start( );
				void stop( );
				void slow( );
				void fast( );
				void enable( );
				void disable( );
				void right( );
				void left( );
				bool isRunning( ) const { return mRunning; }
				bool isFast( ) const { return mFast; }
				bool isEnabled( ) const { return mEnabled; }
				bool isGoingRight( ) const { return mRight; }
			private:
				HAL * const mHAL;
				bool mRunning, mFast, mEnabled, mRight;
		};
	}
}

#endif

